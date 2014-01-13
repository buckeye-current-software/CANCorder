
#include <stdio.h>
#include <stdlib.h>
#include "LinkedList.h"
#include "signal.h"

 
/*
int main(void)
{
  struct my_list*  mt = NULL;
 
  mt = list_new();
  list_add_element(mt, 1);
  list_add_element(mt, 2);
  list_add_element(mt, 3);
  list_add_element(mt, 4); 
   
  list_print(mt);
 
  list_remove_element(mt);
  list_print(mt);
 
  list_free(mt);    always remember to free() the malloc()ed memory
  free(mt);         free() if list is kept separate from free()ing the structure, I think its a good design 
  mt = NULL;       after free() always set that pointer to NULL, C will run havon on you if you try to use a dangling pointer 
 
  list_print(mt);
 
  return 0;
}
*/
 
 
/* Will always return the pointer to my_list */
struct signal_structure* list_add_element(struct my_list* s, struct signal_structure sig)
{
  struct signal_structure* currentSig = mallac(sizeof(struct signal_structure));
  *currentSig = sig;
  

  struct linked_list* p = malloc( 1 * sizeof(*p) );
 
  if( NULL == p )
    {
      fprintf(stderr, "IN %s, %s: malloc() failed\n", __FILE__, "list_add");
      return &currentSig; 
    }
 
  p->signal = sig;
  p->next = NULL;
 
 
  if( NULL == s )
    {
      printf("Queue not initialized\n");
      free(p);
	  free(currentSig);
      return &currentSig;
    }
  else if( NULL == s->head && NULL == s->tail )
    {
      /* printf("Empty list, adding p->num: %d\n\n", p->num);  */
      s->head = s->tail = p;
      return &currentSig;
    }
  else if( NULL == s->head || NULL == s->tail )
    {
      fprintf(stderr, "There is something seriously wrong with your assignment of head/tail to the list\n");
      free(p);
	  free(currentSig);
      return NULL;
    }
  else
    {
      /* printf("List not empty, adding element to tail\n"); */
      s->tail->next = p;
      s->tail = p;
    }
 
  return &currentSig;
}
 
 
/* This is a queue and it is FIFO, so we will always remove the first element */
struct my_list* list_remove_element( struct my_list* s )
{
  struct linked_list* h = NULL;
  struct linked_list* p = NULL;
 
  if( NULL == s )
    {
      printf("List is empty\n");
      return s;
    }
  else if( NULL == s->head && NULL == s->tail )
    {
      printf("Well, List is empty\n");
      return s;
    }
  else if( NULL == s->head || NULL == s->tail )
    {
      printf("There is something seriously wrong with your list\n");
      printf("One of the head/tail is empty while other is not \n");
      return s;
    }
 
  h = s->head;
  p = h->next;
  free(h);
  s->head = p;
  if( NULL == s->head )  s->tail = s->head;   /* The element tail was pointing to is free(), so we need an update */
 
  return s;
}
   
 
/* ---------------------- small helper fucntions ---------------------------------- */
struct my_list* list_free( struct my_list* s )
{
  while( s->head )
    {
      list_remove_element(s);
    }
 
  return s;
}
 
struct my_list* list_new(void)
{
  struct my_list* p = malloc( 1 * sizeof(*p));
 
  if( NULL == p )
    {
      fprintf(stderr, "LINE: %d, malloc() failed\n", __LINE__);
    }
 
  p->head = p->tail = NULL;
   
  return p;
}
 
 
void list_print( const struct my_list* ps )
{
  struct linked_list* p = NULL;
 
  if( ps )
    {
      for( p = ps->head; p; p = p->next )
    {
      list_print_element(p);
    }
    }
 
  printf("------------------\n");
}
 
 
void list_print_element(const struct linked_list* p )
{
  if( p ) 
    {
      //printf("Num = %d\n", p->num);
    }
  else
    {
      //printf("Can not print NULL struct \n");
    }
}
