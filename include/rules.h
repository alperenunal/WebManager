#ifndef RULES_H
#define RULES_H

typedef struct Rule Rule;

Rule *parseRule(const char *request);
void callHandle(Rule *rule, int fd, void *args);

#endif
