#include "server.h"

#include "rules.h"
#include "handles.h"
#include "process.h"

#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netdb.h>

#include <pthread.h>
#include <unistd.h>

typedef struct {
	int fd;
	struct sockaddr_in addr;
	socklen_t len;
} Server;

static Server server;
pthread_mutex_t lock;
ProcessList *procList;

void startServer(int port) {
	procList = initProcList();
	if (procList == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	if ((server.fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	server.addr.sin_addr.s_addr = INADDR_ANY;
	server.addr.sin_family = AF_INET;
	server.addr.sin_port = htons(port);
	server.len = sizeof(server.addr);

	if (bind(server.fd, (struct sockaddr *)&server.addr, server.len) == -1) {
		perror("bind");
		stopServer(EXIT_FAILURE);
	}
}

void stopServer(int status) {
	close(server.fd);
	exit(status);
}

void *serve(void *_fd) {
	int fd = (int)_fd;
	char request[2048] = {0};

	if (recv(fd, request, sizeof(request) - 1, 0) <= 0) {
		perror("receive");
		goto close_socket;
	}

	sendResponse(fd, request);

close_socket:
	close(fd);

	pthread_exit(NULL);
}

void listenAndServe(int n) {
	if (listen(server.fd, n) == -1) {
		perror("listen");
		stopServer(EXIT_FAILURE);
	}

	if (pthread_mutex_init(&lock, NULL)) {
		perror("mutex");
		stopServer(EXIT_FAILURE);
	}

	for (;;) {
		int fd;
		pthread_t tid;

		if ((fd = accept(server.fd, NULL, NULL)) == -1) {
			perror("accept");
			continue;
		}

		if (pthread_create(&tid, NULL, serve, (void *)fd)) {
			perror("thread");
			close(fd);
			continue;
		}
	}
}
