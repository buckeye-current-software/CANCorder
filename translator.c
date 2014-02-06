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
#include <math.h>
#include <libgen.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <net/if.h>
#include <semaphore.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "lib.h"
#include "signal.h"
#include "include/socketcan/can.h"
#include "translator.h"
#include "MessageAVL.h"
#include "SignalAVL.h"
#include "avl.h"
#include "LinkedList.h"

union byteData
{
  uint8_t U8;
  int8_t I8;
  uint16_t U16;
  int16_t I16;
  uint32_t U32;
  int32_t I32;
  uint64_t U64;
  int64_t I64;
  float_t FLOAT;
  double_t DOUBLE;
} byteData;

extern sem_t semaphore;

void translate(tree *message_tree, tree *signal_tree, struct can_frame *frame) {

	uint8_t frameLength = frame->can_dlc;
	int msgID = (int)frame->can_id;
	int i;

	// May be able to simply make the array lengths variable (frame length)
	__u8 tempArray[frameLength];
	uint64_t bitmask = 0;
	__u8 origFrameData[frameLength];
	memcpy(&origFrameData, &frame->data[0], frameLength);

	struct message_node msg_node;

	struct signal_node sig_node;
	struct signal_structure signal;
	struct list_node *node;
	msg_node.key = (char*)malloc(msgID * sizeof(char));
	//msg_node.key = "0";
	sprintf(msg_node.key, "%d", msgID);

	if(is_present(message_tree, &msg_node))
	{
		//printf("Found message in tree");
		get_data(message_tree, &msg_node, sizeof(struct message_node));
		node = msg_node.list->head;
		signal = node->signal;
		//nextNode = node->next;
		while(node != NULL)
		{
			memcpy(&frame->data, &origFrameData[0], frameLength);
			memcpy(&byteData.U64, &frame->data[0], frameLength);

			sig_node.key = (char*)malloc(*signal.id * sizeof(char));
			strcpy(sig_node.key, signal.id);
			get_data(signal_tree, &sig_node, sizeof(struct signal_node));

			if(signal.byteOrder == 0) // May need changed. Look into this
			{
				signal.startBit = 64 - (8*(signal.startBit/8)) - ((8-((signal.startBit+1)%8))%8) - signal.length;
				//Reverse the byte array or start index at end and decrement
				for(i = 0; i < 8; i++)
				{
					tempArray[(frameLength-1)-i] = frame->data[i];
				}
				memcpy(&frame->data,&tempArray[0], frameLength);
			}

			memcpy(&byteData.U64, &frame->data[0], frameLength); // Obtain data and store in union
			byteData.U64 = byteData.U64 >> signal.startBit; // Get relevant signal data
			//Create bitmask
			bitmask = 0;
			for(i = 0; i < signal.length; i ++) {
				bitmask = (bitmask | (1L << i));
			}
			byteData.U64 = byteData.U64 & bitmask;

			//get_data(signal_tree, &sig_node, sizeof(struct signal_node));
			get_signal(signal_tree, &sig_node, sizeof(struct signal_node));
			//sig_node = get_signal(signal_tree, &sig_node, sizeof(struct signal_node));
			signal.dataType = sig_node.signal.dataType; // Due to signals.dataType being different between msg_tree and sig_tree

			//printf("Data type = %d\n", signal.dataType);
			sem_wait(&semaphore);
			if(signal.dataType == 1) // Signed int
			{
				if(signal.length <= 8)
				{
					if(signal.length == 8)
					{

						get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value = (double)byteData.I8;

						//printf("Data: %g\n", get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value);
					}
					else
					{
						get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value = (double)((byteData.I8 << (8 - signal.length)) >> (8-signal.length));
						//printf("Data: %g\n", get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value);
						//sig_node.value = sig_node.value << (8 - signal.length); Can't do this because sig_node.value is a double
						//sig_node.value = sig_node.value >> (8 - signal.length);
					}
				}
				else if(signal.length <= 16)
				{
					if(signal.length == 16)
					{
						get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value = (double)byteData.I16;
						//printf("Data: %g\n", get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value);
					}
					else
					{
						get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value = (double)((byteData.I16 << (16 - signal.length)) >> (16-signal.length));
						//printf("Data: %g\n", get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value);
						//sig_node.value = sig_node.value << (16 - signal.length);
						//sig_node.value = sig_node.value >> (16 - signal.length);
					}

				}
				else if(signal.length <= 32)
				{
					if(signal.length == 32)
					{
						get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value = (double)byteData.I32;
						//printf("Data: %g\n", get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value);
					}
					else
					{
						get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value = (double)((byteData.I32 << (32 - signal.length)) >> (32-signal.length));
						//printf("Data: %g\n", get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value);
						//sig_node.value = sig_node.value << (32 - signal.length);
						//sig_node.value = sig_node.value >> (32 - signal.length);
					}
				}
				else
				{
					if(signal.length == 64)
					{
						get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value = (double)byteData.I64;
						//printf("Data: %g\n", get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value);
					}
					else
					{
						get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value = (double)((byteData.I64 << (64 - signal.length)) >> (64-signal.length));
						//printf("Data: %g\n", get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value);
						//sig_node.value = sig_node.value << (64 - signal.length);
						//sig_node.value = sig_node.value >> (64 - signal.length);
					}
				}
			}
			else if(signal.dataType == 2) // Unsigned int
			{
				if(signal.length <= 8)
				{
					//if(signal.length == 8)
					//{
					get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value = (double)byteData.U8;
					//printf("Data: %g\n", get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value);
					//}
					//else
					//{
						// Binary pad
					//}
				}
				else if(signal.length <= 16)
				{
					//if(signal.length == 16)
					//{
					get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value = (double)byteData.U16;
					//printf("Data: %g\n", get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value);
					//}
					//else
					//{

					//}

				}
				else if(signal.length <= 32)
				{
					//if(signal.length == 32)
					//{
					get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value = (double)byteData.U32;
					//printf("Data: %g\n", get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value);
					//}
					//else
					//{

					//}
				}
				else
				{
					//if(signal.length == 64)
					//{
					get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value = (double)byteData.U64;
					//printf("Data: %g\n", get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value);
					//}
				}
			}
			else if(signal.dataType == 3) // Float
			{
				if(signal.length != 32)
				{
					//sig_node.value = (double)((byteData.FLOAT << (32 - signal.length)) >> (32-signal.length));
					//printf("Data: %f\n", get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value);
				}

				//printf("%g\n", (double)byteData.FLOAT);
				get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value = (double)byteData.FLOAT;
				//printf("Data: %g\n", get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value);
			}
			else //Double
			{
				get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value = byteData.DOUBLE;
				//printf("Data: %g\n", get_signal(signal_tree, &sig_node, sizeof(struct signal_node))->value);
			}
			sem_post(&semaphore);

			if(node->next != NULL)
			{
				signal = node->next->signal;
			}
			node = node->next;
		}
	}
}

