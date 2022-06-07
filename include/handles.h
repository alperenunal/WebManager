#ifndef HANDLES_H
#define HANDLES_H

#include <pthread.h>

typedef void (*HandleFunc)(int fd, char *args);

void indexHandle(int fd, char *args);
void styleHandle(int fd, char *args);
void jsHandle(int fd, char *args);
void infoHandle(int fd, char *args);
void postHandle(int fd, char *args);
void exitHandle(int fd, char *args);

void notFoundHandle(int fd);
void errorHandle(int fd);

extern pthread_mutex_t lock;

#endif
