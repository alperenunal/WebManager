#ifndef SERVER_H
#define SERVER_H

typedef struct Server Server;

void startServer(void);
void stopServer(int status);
void listenAndServe(int n);

#endif
