
/* 
   Parser Outline
   Version 0.1, Buckeye Current 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "signal.h"
#include "LinkedList.h"
#include "avl.h"

struct message_node {
    char key[5];
    struct my_list *list;
};

struct signal_node {
	char key[20];
	struct signal_structure signal;
};

int data_cmp(void *a, void *b)
{
    struct message_node *aa = (struct message_node *) a;
    struct message_node *bb = (struct message_node *) b;

    // Protect against NULL pointer
    // It could generally never happened
    if (!aa || !bb)
        return 0;

    return memcmp(aa->key, bb->key, sizeof(aa->key));
}

// Function that dumps data structure
void data_print(void *d)
{
	/*
	 * Mostly deleted. Only used to initialize a tree
	 */
    struct message_node *dd = (struct message_node *) d;
    if (dd)
    	printf("{ %s => %s }\n", dd->key, dd->list->head->signal->id);
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

    d->key[5] = s->key[5];
    d->list = s->list;
}

void parseFile(char *fileName)
{
	FILE *file;
	/* Open file handler using fileName in read mode */
	file = fopen(fileName, "r");
	if (file == NULL)
	{
		fprintf(stderr, "Can't open %s\n", *fileName);
		exit(1);		
	}

	/* Read a line in at a time */
	char buf[BUFSIZ], messageID[5], tmp[50], startBit[3], length[3], byteOrder[2], dataType[2];
	char *signalID;
	int index, index2 = 0, tmpLength;
	int added_last_msg = 0;
	
	struct my_list* mt = NULL; // Message list (RENAME THIS BECAUSE ITS AWFUL)
	struct message_node msg;
	msg.key[0] = '\0';
	struct signal_node sig_node;
	struct signal_structure sig;
	
	tree *msg_tree = NULL;
	msg_tree = init_dictionnary(data_cmp, data_print, data_delete, data_copy);
	tree *signal_tree = NULL;
	signal_tree = init_dictionnary(data_cmp, data_print, data_delete, data_copy);
	
	
	while(fgets(buf,BUFSIZ,file) != NULL)
	{
		if(strstr(buf, "BO_") != NULL)
		{
			if(msg.key[0] != '\0')
			{
				// add current linked_list and message to Message AVL tree
				msg.list = mt;
				insert_elmt(msg_tree, &msg, sizeof(struct message_node));
			}
			index = 4;
			while(buf[index] != ' ')
			{
				messageID[index-4] = buf[index];
				index++;
			}
			messageID[index-4] = '\0';
			msg.key[0] = messageID[0];
			msg.key[1] = messageID[1];
			msg.key[2] = messageID[2];
			msg.key[3] = messageID[3];
			msg.key[4] = messageID[4];
			mt = list_new();
		}
		if(strstr(buf, "SG_") != NULL)
		{
			index = 5;
			// Store index 5 through * as signal ID in Bike
			while(buf[index] != ' ')
			{
				tmp[index-5] = buf[index];
				index++;
				//malloc?
			}
			tmp[index-5] = '\0';
			tmpLength = strlen(tmp)+1;
			signalID =(char*)malloc(tmpLength * sizeof(char));
			strcpy(signalID, tmp);
			strcpy(sig.id, signalID);
			strcpy(sig_node.key, signalID);
			
			// Move index +3 to skip uselss dbc stuff
			index = index + 3;
			// Store start bit
			while(buf[index] != '|')
			{
				startBit[index2] = buf[index];
				index++;
				index2++;
			}
			startBit[index2] = '\0';
			sig.startBit = atoi(startBit); // Converts string to int
			index2 = 0;
			index++;

			// Store length 
			while(buf[index] != '@')
			{
				length[index2] = buf[index];
				index++;
				index2++;
			}
			length[index2] = '\0';
			sig.length = atoi(length); 
			index2 = 0;
			index++;
			
			// Store byte order
			byteOrder[0] = buf[index];
			index++;
			byteOrder[1] = '\0';
			sig.byteOrder = atoi(byteOrder);
			
			// Store data type
			dataType[0] = buf[index];
			index++;
			dataType[1] = '\0';

			// Find what the dataType string equals and assign correct integer to sig.dataType

			if(dataType[0] == '-')
			{
				sig.dataType = 1; // Represents signed int
			}
			else
			{
				sig.dataType = 2; // Represents unsigned int
			}
			
			// Add signal to linked_list
			sig_node.signal = list_add_element(mt, sig);

			// Add signal to signal node, add signal node to AVL tree
			insert_elmt(signal_tree, &sig_node, sizeof(struct signal_node));
		}
		// Is there a way to do this in C? 
		if(strstr(buf, "SIG_VALTYPE_ ") != NULL)
		{
			if(added_last_msg == 0)
			{
				msg.list = mt;
				insert_elmt(msg_tree, &msg, sizeof(struct message_node));
				added_last_msg = 1;
			}
			index = 15;
			while(buf[index] != ' ')
			{
				tmp[index-15] = buf[index];
				index++;
			}
			tmp[index] = '\0';
			tmpLength = strlen(tmp)+1;
			signalID =(char*)malloc(tmpLength * sizeof(char));
			strcpy(sig_node.key, tmp);

			get_data(signal_tree, &sig_node, sizeof(struct signal_node));

			index = index + 3;
			if(buf[index] == '1')
			{
				sig_node.signal.dataType = 3;
			}
			else
			{
				sig_node.signal.dataType = 4;
			}

		}
	}
	fprintf(stdout, "Message Tree: \n");
	print_tree(msg_tree);
	fprintf(stdout, "Signal Tree: \n");
	print_tree(signal_tree);
	
	fclose(file);
	fprintf(stdout, "DONE!");
}
