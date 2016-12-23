#pragma once

struct clist_l {
    uint8_t *cname;
    struct clist_l *next; 
};
typedef struct clist_l clist;

clist *curr, *head, *root;
