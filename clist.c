#include "clist.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

clist_t *clist_init() {
    clist_t *list = malloc(sizeof(clist_t));
    list->entries = NULL;
    list->size = 0;
    return list;
}

clist_t *clist_add(clist_t *list, char *elem) {
    if(list->entries == NULL) {
        list->entries = malloc(sizeof(char*));
        list->entries[list->size++] = elem;
    } else {
        unsigned size = list->size;
        list->entries = realloc(list->entries, (sizeof(char*) * (++list->size)));
        list->entries[size] = elem;
    }
    return list;
}

bool clist_contains(clist_t *list, char *elem) {
    int i; 
    for(i = 0; i < list->size; i++) {
        if(strcmp(list->entries[i], elem) == 0) {
            return true;
        }
    }
    return false;
}