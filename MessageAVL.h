#include <stdlib.h>
#include <stdio.h>
#include "avl.h"

// Structure we want to store
// key is used to order data
// Using linked_list to store all signals for that message
struct message_node {
    char key[5];
    struct linked_list *list;
};

int data_cmp(void *a, void *b);

void data_print(void *d);

void data_delete(void *d);

void data_copy(void *src, void *dst);

