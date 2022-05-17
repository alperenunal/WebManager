#include "server.h"

#include <signal.h>
#include <getopt.h>

int main(/*int argc, char const *argv[]*/) {
    // getopt(argc, argv, NULL);
    signal(SIGPIPE, SIG_IGN);

    startServer();
    listenAndServe(10);

    return 0;
}
