#include "handles.h"

#include "http.h"
#include "cpu.h"
#include "process.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <unistd.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/sendfile.h>

#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>

typedef struct {
	int refSec;
	int cpuUtil;
	int cpuTemp;
	int entry;
} Settings;

Settings settings = {
	.refSec = 2,
	.cpuUtil = 50,
	.cpuTemp = 50,
	.entry = 5,
};

static void writeLog(const char *log, ...) {
	va_list args;
	va_start(args, log);

	FILE *file;
	if ((file = fopen("log.txt", "a")) == NULL) {
		va_end(args);
		return;
	}

	vfprintf(file, log, args);

	fclose(file);
	va_end(args);
}

void indexHandle(int fd, char *args) {
	(void)args;

	int file = open("static/index.html", O_RDONLY);
	if (file == -1) {
		perror("open");
		return;
	}

	struct stat buf;
	if (fstat(file, &buf) == -1) {
		perror("fstat");
		goto close_file;
	}

	procList->index = 0;

	dprintf(fd, HTTP_OK CT_HTML CRLF);
	sendfile(fd, file, 0, buf.st_size);

close_file:
	close(file);
}

void styleHandle(int fd, char *args) {
	(void)args;

	int file = open("static/style.css", O_RDONLY);
	if (file == -1) {
		perror("open");
		goto close_file;
	}

	struct stat buf;
	if (fstat(file, &buf) == -1) {
		perror("fstat");
		goto close_file;
	}

	dprintf(fd, HTTP_OK CT_CSS CRLF);
	sendfile(fd, file, 0, buf.st_size);

close_file:
	close(file);
}

void jsHandle(int fd, char *args) {
	(void)args;

	int file = open("static/index.js", O_RDONLY);
	if (file == -1) {
		perror("open");
		goto close_file;
	}

	struct stat buf;
	if (fstat(file, &buf) == -1) {
		perror("fstat");
		goto close_file;
	}

	dprintf(fd, HTTP_OK CT_JS CRLF);
	sendfile(fd, file, 0, buf.st_size);

close_file:
	close(file);
}

void infoHandle(int fd, char *args) {
	(void)args;

	long uptime;
	double temp;
	double load1, load5, load15;
	int ncpu = get_nprocs();
	double usage[ncpu];

	temp = getTemp();

	if (!getCPUUsage(ncpu, usage) || !getUptime(&uptime) ||
		!getAvgLoad(&load1, &load5, &load15)) {
		errorHandle(fd);
		return;
	}

	struct tm *uptm = gmtime((time_t *)&uptime);
	if (uptm == NULL) {
		perror("gmtime");
		errorHandle(fd);
		return;
	}

	dprintf(fd, HTTP_OK CT_JSON CRLF);

	dprintf(fd, "{");

	dprintf(fd, "\"uptime\":\"%02d:%02d:%02d\",", uptm->tm_hour, uptm->tm_min,
			uptm->tm_sec);

	dprintf(fd, "\"temp\":%.1lf,", temp);

	if (temp > settings.cpuTemp) {
		writeLog("CPU Temperature is high: %.1lf\n", temp);
	}

	dprintf(fd, "\"loadavg\":[%.2lf, %.2lf, %.2lf],", load1, load5, load15);

	dprintf(fd, "\"loads\":[");
	for (int i = 0; i < ncpu; ++i) {
		if (i == ncpu - 1) {
			dprintf(fd, "%.1lf", usage[i]);
			break;
		}

		dprintf(fd, "%.1lf,", usage[i]);

		if (usage[i] > settings.cpuUtil) {
			writeLog("CPU#%d usage is high: %.1lf\n", i + 1, usage[i]);
		}
	}
	dprintf(fd, "],");

	dprintf(fd, "\"sec\":%d,", settings.refSec);

	dprintf(fd, "\"entry\":%d,", settings.entry);

	dprintf(fd, "\"procs\":[");
	for (; procList->index < procList->len; procList->index++) {
		Process *proc = procList->procs[procList->index];

		if (procList->index == procList->len - 1) {
			dprintf(fd, "[\"%s\", %d, %d]", proc->command, proc->pid,
					CPU_COUNT(&proc->affinity));
			break;
		}
		dprintf(fd, "[\"%s\", %d, %d],", proc->command, proc->pid,
				CPU_COUNT(&proc->affinity));
	}
	dprintf(fd, "],");

	FILE *file;
	if ((file = fopen("log.txt", "r")) == NULL) {
		errorHandle(fd);
		return;
	}

	char logLine[1024] = {0};

	dprintf(fd, "\"logs\":[");
	for (int i = 0; fgets(logLine, sizeof(logLine), file); ++i) {
		if (i > 0) {
			dprintf(fd, ",");
		}
		dprintf(fd, "\"%.*s\"", (int)strlen(logLine) - 1, logLine);
	}
	dprintf(fd, "]");

	dprintf(fd, "}");
}

static int getCommand(char *cmd) {
	if (!strcmp(cmd, "sys-cmd")) {
		return 1;
	} else if (!strcmp(cmd, "run-cmd")) {
		return 2;
	} else if (!strcmp(cmd, "kill-cmd")) {
		return 3;
	} else if (!strcmp(cmd, "sec-set")) {
		return 4;
	} else if (!strcmp(cmd, "util-set")) {
		return 5;
	} else if (!strcmp(cmd, "temp-set")) {
		return 6;
	} else if (!strcmp(cmd, "entry-set")) {
		return 7;
	} else if (!strcmp(cmd, "clear-logs")) {
		return 8;
	}
	return 0;
}

static void sysCmd(int fd, char *cmd) {
	switch (*cmd) {
	case 's':
		system("shutdown -h");
		writeLog("Shutdown called\n");
		break;
	case 'r':
		system("shutdown -r");
		writeLog("Reboot called\n");
		break;
	case 'c':
		system("shutdown -c");
		writeLog("Shutdown/Reboot cancelled\n");
		break;
	default:
		notFoundHandle(fd);
		return;
	}

	indexHandle(fd, NULL);
}

static void runCmd(int fd, char *cmd) {
	int i;
	char *args[10] = {NULL};
	char *arg, *rest;

	arg = strtok_r(cmd, "+", &rest);
	for (i = 0; arg && i < 10; ++i) {
		args[i] = arg;
		arg = strtok_r(NULL, "+", &rest);
	}
	args[i] = NULL;

	Process *proc;
	cpu_set_t cpu;
	pid_t cpi = fork();

	switch (cpi) {
	default:
		sched_getaffinity(cpi, sizeof(cpu), &cpu);
		proc = initProc(strdup(args[0]), cpi, cpu);
		addProcList(procList, proc);
		writeLog("Process %s with ID %d started\n", args[0], cpi);
		break;
	case 0:
		execvp(args[0], args);
		break;
	case -1:
		break;
	}

	indexHandle(fd, NULL);
}

static void killCmd(int fd, char *cmd) {
	int pid = atoi(cmd);
	if (pid == 0) {
		for (int i = 0; i < procList->len; ++i) {
			Process *proc = procList->procs[i];
			if (!strcmp(proc->command, cmd)) {
				kill(proc->pid, SIGKILL);
				writeLog("Process %s with ID %d killed\n", proc->command,
						 proc->pid);
				indexHandle(fd, NULL);
				return;
			}
		}

		writeLog("Process %s not found int process table\n", cmd);
	} else {
		for (int i = 0; i < procList->len; ++i) {
			Process *proc = procList->procs[i];
			if (proc->pid == pid) {
				kill(proc->pid, SIGKILL);
				writeLog("Process %s with ID %d killed\n", proc->command,
						 proc->pid);
				indexHandle(fd, NULL);
				return;
			}
		}

		writeLog("Process with ID %d not found int process table\n", pid);
	}
}

static void secSet(int fd, char *val) {
	int sec = atoi(val);

	pthread_mutex_lock(&lock);
	settings.refSec = (sec >= 1) ? sec - 1 : 2;
	pthread_mutex_unlock(&lock);

	indexHandle(fd, NULL);
}

static void utilSet(int fd, char *val) {
	int util = atoi(val);

	pthread_mutex_lock(&lock);
	settings.cpuUtil = (util > 0) ? util : 50;
	pthread_mutex_unlock(&lock);

	indexHandle(fd, NULL);
}

static void tempSet(int fd, char *val) {
	int temp = atoi(val);

	pthread_mutex_lock(&lock);
	settings.cpuUtil = (temp > 0) ? temp : 50;
	pthread_mutex_unlock(&lock);

	indexHandle(fd, NULL);
}

static void entrySet(int fd, char *val) {
	int entry = atoi(val);

	pthread_mutex_lock(&lock);
	settings.entry = (entry > 0) ? entry : 5;
	pthread_mutex_unlock(&lock);

	indexHandle(fd, NULL);
}

static void clearLog(int fd) {
	FILE *file = fopen("log.txt", "w");

	if (file) {
		fprintf(file, "Logs cleared\n");
		fclose(file);
	}

	indexHandle(fd, NULL);
}

void postHandle(int fd, char *args) {
	char *key, *value;

	key = strtok_r(args, "=", &value);

	int cmd = getCommand(key);
	switch (cmd) {
	case 0:
		notFoundHandle(fd);
		break;
	case 1:
		sysCmd(fd, value);
		break;
	case 2:
		runCmd(fd, value);
		break;
	case 3:
		killCmd(fd, value);
		break;
	case 4:
		secSet(fd, value);
		break;
	case 5:
		utilSet(fd, value);
		break;
	case 6:
		tempSet(fd, value);
		break;
	case 7:
		entrySet(fd, value);
		break;
	case 8:
		clearLog(fd);
		break;
	}
}

void notFoundHandle(int fd) {
	int file;
	struct stat buf;

	if ((file = open("static/404.html", O_RDONLY)) == -1) {
		perror("open");
		return;
	}

	if (fstat(file, &buf) == -1) {
		perror("fstat");
		goto close_file;
	}

	dprintf(fd, HTTP_NF CT_HTML CRLF);
	sendfile(fd, file, 0, buf.st_size);

close_file:
	close(file);
}

void errorHandle(int fd) {
	int file;
	struct stat buf;

	if ((file = open("static/500.html", O_RDONLY)) == -1) {
		perror("open");
		return;
	}

	if (fstat(file, &buf) == -1) {
		perror("fstat");
		goto close_file;
	}

	dprintf(fd, HTTP_IE CT_HTML CRLF);
	sendfile(fd, file, 0, buf.st_size);

close_file:
	close(file);
}

void exitHandle(int fd, char *args) {
	(void)fd;
	(void)args;
	freeProcList(procList);
	exit(EXIT_SUCCESS);
}
