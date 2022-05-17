#include "handles.h"

#include "http.h"

#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/sendfile.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>

#define send1(fd, resp) send(fd, resp, sizeof(resp) - 1, 0)
#define send2(fd, resp) send(fd, resp, strlen(resp), 0)

void indexHandle(int fd, void *args) {
    (void)args;

    int file = open("static/index.html", O_RDONLY);
    if (file == -1) {
        perror("open");
        goto close_file;
    }

    struct stat buf;
    if (fstat(file, &buf) == -1) {
        perror("fstat");
        goto close_file;
    }

    send1(fd, HTTP_OK CT_HTML CRLF);
    sendfile(fd, file, 0, buf.st_size);

close_file:
    close(file);
}

void styleHandle(int fd, void *args) {
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

    send1(fd, HTTP_OK CT_CSS CRLF);
    sendfile(fd, file, 0, buf.st_size);

close_file:
    close(file);
}

void jsHandle(int fd, void *args) {
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

    send1(fd, HTTP_OK CT_JS CRLF);
    sendfile(fd, file, 0, buf.st_size);

close_file:
    close(file);
}

void numberHandle(int fd, void *args) {
    (void)args;

    static int x = 0;
    char resp[100] = {0};

    sprintf(resp, "%d", x);

    send1(fd, HTTP_OK CT_TXT CRLF);
    send2(fd, resp);

    x++;
}

void notFoundHandle(int fd) {
    int file = open("static/404.html", O_RDWR);
    struct stat buf;
    fstat(file, &buf);

    send1(fd, HTTP_NF CT_HTML CRLF);
    sendfile(fd, file, 0, buf.st_size);

    close(file);
}
