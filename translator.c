/*
 * translator.c
 *
 *  Created on: Jan 18, 2014
 *      Author: Sean
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "lib.h"
#include "include/socketcan/can.h"
#include "translator.h"
#include "MessageAVL.h"
#include "SignalAVL.h"
#include "avl.h"
#include "LinkedList.h"

union le_long_long_array_u
{
  uint8_t byte[8];
  uint64_t longlong;
} le_long_long_array;

void translate(tree *message_tree, tree *signal_tree, struct canfd_frame *frame) {
	int msgID = (int)frame->can_id;
	char byteArray[CANFD_MAX_DLEN] = frame->data; //This will need fixed
	char tmpArray[CANFD_MAX_DLEN] = {0, 0, 0, 0, 0, 0, 0, 0};

	long bitmask;
	long long bitData;
	unsigned long long unsignedData;
	int power; //Please look into this again and it's use in bitmasking

	struct message_node msg_node;
	struct signal_node sig_node;
	struct signal_structure signal;
	struct list_node *node, *nextNode;
	msg_node.key = (char*)malloc(msgID * sizeof(char));
	strcmp(msg_node.key, msgID);
	if(is_present(message_tree, &msg_node))
	{
		get_data(message_tree, &msg_node, sizeof(struct message_node));
		node = msg_node.list->head;
		signal = node->signal;
		nextNode = node->next;
		while(nextNode != NULL)
		{
			sig_node.key = (char*)malloc(signal.id * sizeof(char));
			strcmp(sig_node.key, signal.id);
			if(signal.byteOrder == 0) // May need changed. Look into this
			{
				//Reverse the byte array or start index at end and decrement
				for(int i = 0; i < 7; i++)
				{
					tmpArray[i] = byteArray[7-i];
				}
				byteArray = tmpArray;
			}

			bitmask = 0;
			power = signal.startBit;
			for(int i = 0; i < signal.length; i++)
			{
				bitmask = (bitmask | (1 << power));
			}
			//ld for signed long, lu for unsigned
			printf("Bitmask: %ld\n", bitmask); // Sanity check
			if(signal.dataType == 2)
			{
				for(int i = 0; i < 8; i++)
				{
					bitData |= ( (long long)byteArray[i]) << (i*8);
				}
				printf("Unsigned Data: %ld\n", unsignedData);

				unsignedData = unsignedData & bitmask;
				unsignedData = unsignedData >> signal.startBit;
				printf("Modified unsigned data: %ld\n", unsignedData);
			}
			else
			{
				for(int i = 0; i < 8; i++)
				{
					bitData |= ( (long long)byteArray[i]) << (i*8);
				}
				printf("Data: %ld\n", bitData); // Sanity check to make sure endianness correct

				bitData = bitData & bitmask;
				bitData = bitData >> signal.startBit;
				printf("Modifed data: %ld\n", bitData);
			}



			if(signal.byteOrder == 1)
			{
				sig_node.value = bitData;
			}
			if(signal.byteOrder == 2)
			{
				sig_node.value = unsignedData;
			}
			if(signal.byteOrder > 3)
			{
				//Float or double
				(double)bitData;
			}

			// Only need to convert bitData to correct dataType now?



		}
	}
}

