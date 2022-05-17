#include "rules.h"

#include "http.h"
#include "handles.h"

#include <stdlib.h>
#include <string.h>

#define numberOfRules (sizeof(rules) / sizeof(Rule))

struct Rule {
    Method method;
    char *url;
    handleFunc handle;
};

static void freeRule(Rule *rule) {
    if (rule == NULL) {
        return;
    }

    free(rule->url);
    free(rule);
}

const Rule rules[] = {
    {.method = GET, .url = "/", .handle = indexHandle},
    {.method = GET, .url = "/style.css", .handle = styleHandle},
    {.method = GET, .url = "/index.js", .handle = jsHandle},
    {.method = GET, .url = "/number", .handle = numberHandle},
};

Rule *parseRule(const char *request) {
    char *method = NULL;
    char *url = NULL;

    for (int i = 0, j = 0; request[i]; ++i, ++j) {
        if (request[i] == ' ') {
            if (!method) {
                method = malloc(j + 1);
                strncpy(method, request, j);
                method[j] = '\0';
                j = -1;
            } else if (!url) {
                url = malloc(j + 1);
                strncpy(url, request + i - j, j);
                url[j] = '\0';
                break;
            }
        }
    }

    Rule *rule = malloc(sizeof(*rule));

    if (!strcmp(method, "GET")) {
        rule->method = GET;
        free(method);
    } else if (!strcmp(method, "POST")) {
        rule->method = POST;
        free(method);
    } else {
        free(method);
        free(url);
        free(rule);
        return NULL;
    }

    rule->url = url;
    rule->handle = NULL;

    return rule;
}

void callHandle(Rule *rule, int fd, void *args) {
    if (rule == NULL) {
        notFoundHandle(fd);
        return;
    }

    Method method = rule->method;
    const char *url = rule->url;

    for (size_t i = 0; i < numberOfRules; ++i) {
        Rule r = rules[i];
        if (method == r.method && !strcmp(url, r.url)) {
            r.handle(fd, args);
            freeRule(rule);
            return;
        }
    }

    notFoundHandle(fd);
    freeRule(rule);
}
