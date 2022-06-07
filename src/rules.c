#include "rules.h"

#include "http.h"
#include "handles.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define arraySize(arr) (int)(sizeof(arr) / sizeof((arr)[0]))

typedef struct {
	Method method;
	char *url;
	HandleFunc handle;
} Rule;

static const Rule rules[] = {
	{.method = GET, .url = "/", .handle = indexHandle},
	{.method = POST, .url = "/", .handle = postHandle},
	{.method = GET, .url = "/info", .handle = infoHandle},
	{.method = GET, .url = "/index.js", .handle = jsHandle},
	{.method = GET, .url = "/style.css", .handle = styleHandle},
	{.method = POST, .url = "/exit", .handle = exitHandle},
};

static void callHandle(Rule rule, int fd, char *arg) {
	Method method = rule.method;
	const char *url = rule.url;

	for (int i = 0; i < arraySize(rules); ++i) {
		Rule r = rules[i];
		if (method == r.method && !strcmp(url, r.url)) {
			r.handle(fd, arg);
			return;
		}
	}

	notFoundHandle(fd);
}

void sendResponse(int fd, char *request) {
	Rule rule;
	char *method = NULL, *url = NULL, *arg = NULL;
	char *rest = NULL;

	method = strtok_r(request, " ", &rest);
	url = strtok_r(NULL, " ", &rest);

	if (!strcmp(method, "GET")) {
		rule.method = GET;
	} else if (!strcmp(method, "POST")) {
		rule.method = POST;
		while (*rest) {
			arg = strtok_r(NULL, "\r\n", &rest);
		}
	} else {
		notFoundHandle(fd);
		return;
	}

	rule.url = url;
	rule.handle = NULL;

	callHandle(rule, fd, arg);
}
