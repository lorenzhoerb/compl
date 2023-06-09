#ifndef CLIST
#define CLIST
#include <stdbool.h>

typedef struct clist {
    unsigned size;
    char **entries;
} clist_t;

clist_t *clist_init();

clist_t *clist_add(clist_t *list, char *elem);

bool clist_contains(clist_t *list, char *elem);

#endif