
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
#include "MessageAVL.h"
#include "SignalAVL.h"



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
	int index, index2 = 0, tmpLength;
	int added_last_msg = 0;
	
	struct my_list* mt = NULL; // Message list (RENAME THIS BECAUSE ITS AWFUL)
	struct message_node msg;
	msg.key[0] = '\0';
	struct signal_node sig_node;
	struct signal_structure sig;
	
	unsigned int test; // remove me after parser works

	tree *msg_tree = initialize_msg_avl();
	tree *signal_tree = initialize_signal_avl();
	
	
	while(fgets(buf,BUFSIZ,file) != NULL)
	{
		if(strstr(buf, "BO_") != NULL)
		{
			if(msg.key[0] != '\0')
			{
				// add current linked_list and message to Message AVL tree
				msg.list = mt; //Getting a warning before about "assignment from incompatible pointer type"
				insert_elmt(msg_tree, &msg, sizeof(struct message_node));
				list_free(mt);
				free(mt);
				mt = NULL;
				fprintf(stdout, "Inserting message node into msg tree \n");
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
			}
			tmp[index-5] = '\0';
			tmpLength = strlen(tmp)+1;
			signalID =(char*)malloc(tmpLength * sizeof(char));
			strcpy(signalID, tmp);
			strcpy(sig.id, signalID);
			strcpy(sig_node.key, signalID);
			printf("Key for sig_node: %s\n", sig_node.key);
			
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

			fprintf(stdout, "Inserting node into signal tree \n");
			// Add signal to signal node, add signal node to AVL tree
			test = insert_elmt(signal_tree, &sig_node, sizeof(struct signal_node));
			free(signalID);
			fprintf(stdout, "node: %i \n", test);

		}
		// Is there a way to do this in C? 
		if(strstr(buf, "SIG_VALTYPE_ ") != NULL)
		{
			if(added_last_msg == 0)
			{
				msg.list = mt;
				insert_elmt(msg_tree, &msg, sizeof(struct message_node));
				list_free(mt);
				free(mt);
				mt = NULL;
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
			free(signalID);

		}
	}
	fprintf(stdout, "Message Tree: \n");
	print_tree(msg_tree);
	fprintf(stdout, "Signal Tree: \n");
	print_tree(signal_tree);
	
	fclose(file);
	fprintf(stdout, "DONE!");
}
