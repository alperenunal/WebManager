#include "process.h"

#include <stdlib.h>

Process *initProc(char *command, int pid, cpu_set_t aff) {
	Process *proc = malloc(sizeof(*proc));
	if (proc == NULL) {
		return NULL;
	}

	proc->command = command;
	proc->pid = pid;
	proc->affinity = aff;

	return proc;
}

void freeProc(Process *proc) {
	if (proc != NULL) {
		free(proc->command);
		free(proc);
	}
}

ProcessList *initProcList(void) {
	ProcessList *list = malloc(sizeof(*list));
	if (list == NULL) {
		return NULL;
	}

	list->index = 0;
	list->len = 0;
	list->size = 8;
	list->procs = calloc(list->size, sizeof(*list->procs));

	if (list->procs == NULL) {
		free(list);
		return NULL;
	}

	return list;
}

int addProcList(ProcessList *list, Process *proc) {
	if (list->len + 1 >= list->size) {
		int size = list->size * 2;
		Process **tmp = realloc(list->procs, size * sizeof(*tmp));

		if (tmp == NULL) {
			return 0;
		}

		list->size = size;
		list->procs = tmp;
	}

	list->procs[list->len++] = proc;
	return 1;
}

void freeProcList(ProcessList *list) {
	for (int i = 0; i < list->len; ++i) {
		freeProc(list->procs[i]);
	}
	free(list->procs);
	free(list);
}
