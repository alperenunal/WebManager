#ifndef HANDLES_H
#define HANDLES_H

typedef void (*handleFunc)(int fd, void *args);

void indexHandle(int fd, void *args);
void styleHandle(int fd, void *args);
void jsHandle(int fd, void *args);
void numberHandle(int fd, void *args);
void notFoundHandle(int fd);

#endif
