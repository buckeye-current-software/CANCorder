
/* 
   Parser Outline
   Version 0.1, Buckeye Current 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "signal.h"
#include "MessageAVL.h"
#include "LinkedList.h"


void parseFile(char *fileName)
{
	FILE *file;
	/* Open file handler using fileName in read mode */
	file = fopen(fileName, "r");
	if (file == NULL)
	{
		fprintf(stderr, "Can't open %s\n", fileName);
		exit(1);		
	}

	/* Read a line in at a time */
	char buf[BUFSIZ], messageID[5], tmp[50], startBit[3], length[3], byteOrder[2], dataType[2];
	char *signalID;
	int index, index2 = 0;
	
	struct my_list*  mt = NULL;
	struct message_node msg;
	struct signal_structure sig;
	
	tree *avl_tree = NULL;
	avl_tree = init_dictionnary(data_cmp, data_print, data_delete, data_copy);
	
	
	while(fgets(buf,BUFSIZ,file) != NULL)
	{
		process_line(buf);
		if(strstr(buf, "BO_") != NULL)
		{
			if(msg.key != NULL)
			{
				// add current linked_list and message to Message AVL tree
				msg.list = mt;
				insert_elmt(avl_tree, &msg, sizeof(struct message_node));
			}
			index = 5;	
			while(buf[index] != ' ')
			{
				messageID[index-5] = buf[index];
				index++;
			}
			messageID[index-5] = '\0';
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
			signalID = malloc(strlen(tmp));
			sig.id = signalID;
			
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
			if(dataType == "0....")
			{
			
			}
			else if(...)
			{
			
			}
			
			
			list_add_element(mt, sig);
		}
		// Is there a way to do this in C? 
		if(strstr(buf, "SIG_VALTYPE_") != NULL)
		{
			
		}
	}
	
	
	//fclose(fp);
}
