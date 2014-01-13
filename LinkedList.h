/*
 * LinkedList.h
 *
 *  Created on: Jan 12, 2014
 *      Author: Sean
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include "signal.h"

 //Stores signals
struct linked_list //rename to list node
{
  struct signal_structure signal;
  struct linked_list* next;
};


struct my_list
{
  struct linked_list* head;
  struct linked_list* tail;
};


struct signal_structure* list_add_element( struct my_list*, struct signal_structure);
struct my_list* list_remove_element( struct my_list*);


struct my_list* list_new(void);
struct my_list* list_free( struct my_list* );

void list_print( const struct my_list* );
void list_print_element(const struct linked_list* );

#endif /* LINKEDLIST_H_ */
