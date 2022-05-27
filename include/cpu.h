#ifndef CPU_H
#define CPU_H

int getUptime(long *uptime);
int getAvgLoad(double *load1, double *load5, double *load15);
int getCPUUsage(int ncpu, double *usage);

#endif
