#include "MessageAVL.h"

// Function that compares two struct data
int data_cmp(void *a, void *b)
{
    struct message_node *aa = (struct message_node *) a;
    struct message_node *bb = (struct message_node *) b;

    // Protect against NULL pointer
    // It could generally never happened
    if (!aa || !bb)
        return 0;

    return strcmp(aa->key, bb->key);
    //return aa->key - bb->key;
}

// Function that dumps data structure
void data_print(void *d)
{
    struct message_node *dd = (struct message_node *) d;

    //if (dd)
        //printf("{ %d => %d }\n", dd->key, dd->value);
}

// Function that delete a data structure
void data_delete(void *d)
{
    struct message_node *dd = (struct message_node *) d;

    if (dd) {
        // You can put here all additional needed
        // memory deallocation
        free(dd);
    }
}

// Function that copy data structure
void data_copy(void *src, void *dst)
{
    struct message_node *s = (struct message_node *) src;
    struct message_node *d = (struct message_node *) dst;

    //d->key = s->key;
    //d->value = s->value;
}

/*
int main(int argc, char **argv)
{
    tree *avl_tree = NULL;
    struct message_node tmp;
    unsigned result;

    (void) argc;
    (void) argv;

    // Initialize a new tree with our three previously defined
    // functions to store data structure.
    avl_tree = init_dictionnary(data_cmp, data_print, data_delete, data_copy);

    tmp.key = 42;
    tmp.value = 4242;

    // Add element {42, 4242} in our tree.
    result = insert_elmt(avl_tree, &tmp, sizeof(struct message_node));
    // Here result is equal to 1 since there is only 1 element in tree.
    printf("Result after first insert: %d\n", result);

    // Dump tree to stdout with data_print function
    print_tree(avl_tree);

    // For all search function, the only value needed in tmp structure
    // is key field.
    tmp.key = 20;
    tmp.value = 0;

    if (!is_present(avl_tree, &tmp))
        printf("Key 20 is not found.\n");

    tmp.key = 42;
    if (is_present(avl_tree, &tmp))
        printf("Key 42 exist in tree.\n");

    if (get_data(avl_tree, &tmp, sizeof(struct message_node)))
        printf("Now, tmp.key is equal to 4242\n");

    delete_node(avl_tree, &tmp);
    if (!is_present(avl_tree, &tmp))
        printf("Key 42 does not exist anymore.\n");

    // Free all memory
    delete_tree(avl_tree);

    return 0;
}
*/
