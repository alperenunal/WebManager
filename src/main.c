#include "server.h"

#include <stdio.h>
#include <stdlib.h>

#include <getopt.h>
#include <signal.h>

#define HELP                                                                   \
	"Usage:\twebmanager [-h | -p <port>]\n"                                    \
	"\t-h: prints this message and exits\n"                                    \
	"\t-p: specifies the port number (default is 8000)\n"

int main(int argc, char const *argv[]) {
	int opt;
	int port = 8000;

	while ((opt = getopt(argc, argv, "hp:")) != -1) {
		switch (opt) {
		case 'h':
			printf(HELP);
			return 0;
		case 'p':
			port = atoi(optarg);
			port = (port >= 0 && port <= 0xFFFF) ? port : 8000;
			break;
		default:
			fprintf(stderr, HELP);
			return 0;
		}
	}

	signal(SIGPIPE, SIG_IGN);
	startServer(port);
	listenAndServe(10);

	return 0;
}
