
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

extern tree *msg_tree;
extern tree *signal_tree;
int maxSignalSize;


void parseFile(char *fileName)
{
	maxSignalSize = 0;
	FILE *file;
	/* Open file handler using fileName in read mode */
	file = fopen(fileName, "r");
	if (file == NULL)
	{
		fprintf(stderr, "Can't open %s\n", fileName);
		exit(1);		
	}

	/* Read a line in at a time */
	char buf[BUFSIZ], messageID[10], tmp[50], startBit[3], length[3], byteOrder[2], dataType[2];
	char *signalID;
	int index, index2 = 0, tmpLength; // Variables used to store signalID's into smaller arrays
	int first_insert_skipped = 0;     // Skips inserting the first message since no signals have been associated to it yet (it's added later)
	int added_last_msg = 0;           // Used to save the last message in the file
	
	struct my_list* signal_linked_list = NULL; // Linked list of signals all in a single message
	struct message_node msg;
	struct signal_node sig_node; // The nodes that will go into the signal AVL tree
	struct signal_structure sig; // An actual signal structure
	struct signal_structure *currentSig;
	//msg.key[0] = '\0';           // Simply prevents adding an empty message into the message tree down below
	//int msgNodes = 0, signalNodes = 0;
	
	while(fgets(buf,BUFSIZ,file) != NULL)
	{
		if(strstr(buf, "BO_") != NULL)
		{
			if(first_insert_skipped == 1) //msg.key isn't NULL since it is a pointer to the thing in message_node
			{
				// add current linked_list and message to Message AVL tree
				msg.list = signal_linked_list; //Getting a warning before about "assignment from incompatible pointer type"
				printf("MSG: %s\n", msg.key);
				insert_elmt(msg_tree, &msg, sizeof(struct message_node));

				/*
				 * Frees memory from the linked_list after it has been copied and storied into message_avl
				 *
				 * WARNING: These three lines destroy the list stored in the message tree. The lists should not be cleared here.
				 * This will cause a large memory leak on program exit since all the data in list is never freed. Solution not found
				 */

				//list_free(signal_linked_list);
				//free(signal_linked_list);
				//signal_linked_list = NULL;
			}
			first_insert_skipped = 1; // Start adding messages to tree after first msg in file found and skipped

			index = 4;
			while(buf[index] != ' ')
			{
				messageID[index-4] = buf[index];
				index++;
			}
			messageID[index-4] = '\0';
			tmpLength = strlen(messageID) + 1;
			msg.key = (char*)malloc(tmpLength * sizeof(char));
			strcpy(msg.key, messageID);

			signal_linked_list = list_new();
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

			// Allocate smallest char array possible to store signal_ID
			tmpLength = strlen(tmp)+1;
			if(tmpLength > maxSignalSize)
			{
				maxSignalSize = tmpLength;
			}
			signalID =(char*)malloc(tmpLength * sizeof(char));
			sig_node.key = (char*)malloc(tmpLength * sizeof(char));
			sig.id = (char*)malloc(tmpLength * sizeof(char));
			if(signalID == NULL || sig_node.key == NULL || sig.id == NULL)
			{
				printf("MALLOC IS NULL\n");
			}
			strcpy(signalID, tmp);
			strcpy(sig.id, signalID);
			strcpy(sig_node.key, signalID);
			
			// Move index +3 to skip useless dbc formatting stuff
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
			// Move to the signal's unit and store it
			while(buf[index] != '"')
			{
				index ++;
			}
			index ++;
			index2 = 0;
			while(buf[index] != '"')
			{
				tmp[index2] = buf[index];
				index2++;
				index++;
			}
			tmp[index2] = '\0';
			// SAVE MEMORY, SAVE THE ENVIRONMENT

			if(index2 > 0)
			{
				sig.unit = (char*)malloc(index2 * sizeof(char));
				strcpy(sig.unit, tmp);
			}
			// Add signal to linked_list
			sig_node.signal = list_add_element(signal_linked_list, sig);
			fprintf(stdout, "Inserting node into signal tree \n");
			// Add signal to signal node, add signal node to AVL tree
			insert_elmt(signal_tree, &sig_node, sizeof(struct signal_node));
			free(signalID);
		}

		if(strstr(buf, "SIG_VALTYPE_ ") != NULL)
		{
			int nameIndex = 0;
			index = 13;
			while(buf[index] != ' ')
			{
				index++;
			}
			index++;
			memset(tmp,0,strlen(tmp));
			while(buf[index] != ' ')
			{
				tmp[nameIndex] = buf[index];
				index++;
				nameIndex++;
			}
			tmp[nameIndex] = '\0';
			tmpLength = strlen(tmp)+1;
			sig_node.key = (char*)malloc(tmpLength * sizeof(char));
			strcpy(sig_node.key, tmp);

			get_data(signal_tree, &sig_node, sizeof(struct signal_node));
			delete_node(signal_tree, &sig_node);
			index = index + 3;
			if(buf[index] == '1')
			{
				sig_node.signal.dataType = 3;
			}
			else
			{
				sig_node.signal.dataType = 4;
			}
			if (!is_present(signal_tree, &sig_node))
			{
				printf("Signal was indeed deleted from tree");
			}
			insert_elmt(signal_tree, &sig_node, sizeof(struct signal_node));
		}
	}
	msg.list = signal_linked_list;
	insert_elmt(msg_tree, &msg, sizeof(struct message_node));
	added_last_msg = 1;

	fprintf(stdout, "Message Tree: \n"); // Used for debugging. Make sure everything is in the tree
	print_tree(msg_tree);
	fprintf(stdout, "Signal Tree: \n");
	print_tree(signal_tree);
	
	fclose(file);
}
