#ifndef PROCESS_H
#define PROCESS_H

#include <sched.h>

typedef struct {
	char *command;
	int pid;
	cpu_set_t affinity;
} Process;

typedef struct {
	int index;
	int len;
	int size;
	Process **procs;
} ProcessList;

Process *initProc(char *command, int pid, cpu_set_t aff);
void freeProc(Process *proc);

ProcessList *initProcList(void);
int addProcList(ProcessList *list, Process *proc);
void freeProcList(ProcessList *list);

extern ProcessList *procList;

#endif
