/*
 * main.c
 *
 *  Created on: Jan 15, 2014
 *      Author: Sean
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <libgen.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>


#include <sys/socket.h>
#include <linux/netlink.h>

#include <sys/ioctl.h>
#include <sys/uio.h>
#include <net/if.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include "lib.h"

#include "parser.h"
#include "avl.h"
#include "MessageAVL.h"
#include "SignalAVL.h"
#include "datalogger.h"
#include "translator.h"
#include "LinkedList.h"

tree *msg_tree;
tree *signal_tree;
sem_t semaphore, can_semaphore;
FILE *f;
char logString[15];

int keepRunning = 1;

void my_handler(int dummy) {
    keepRunning = 0;
}

/*
 * Thread dedicated to sending out the CANcorder heartbeat
 */
void *txcanthread(int cansock) {

	struct canfd_frame txmsg;
	while(keepRunning)
	{
		sleep(1);
		txmsg.can_id  = 0xAA; 		// CANcorder heartbeat address
		txmsg.len = 0; 				// No data associated with the heartbeat
		sem_wait(&can_semaphore); 	// Wait until we aren't reading in a message (may not be necessary)
		write(cansock, &txmsg, sizeof(struct can_frame));
		sem_post(&can_semaphore);
	}
	return NULL;
}

void *logthread()
{
	// Generate a default log file name before it is named. log1.csv, log2.csv, etc..
	int logNum = 1;
	char lognumstr[4];
	sprintf(lognumstr, "%d", logNum);
	strcpy(logString, "log");
	strcat(logString, lognumstr);
	strcat(logString, ".csv");
	while(access(logString, F_OK ) != -1)
	{
		logNum ++;
		sprintf(lognumstr, "%d", logNum);
		strcpy(logString, "log");
		strcat(logString, lognumstr);
		strcat(logString, ".csv");
	}
	// Open the file with write permissions
	f = fopen(logString, "w");
	if(f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}
	fprintf(f, "Runtime,System_Time,");			// Start inserting headers
	explore_tree(signal_tree, insert_headers); 	// Generates headers once (can change to once in so many lines written)
	fprintf(f, "\n");
	while(keepRunning)
	{
		usleep(25000); // Will cause "memory leak" since it can write to the file everything this quickly. Won't crash program
		data_log(signal_tree); // Datalog values in the signal tree
		fflush(f);
	}
	fclose(f);
	return NULL;
}

int main()
{
	struct sigaction sigIntHandler;
	// Event handlers for use in debugging (Catch ctrl + C)
	sigIntHandler.sa_handler = my_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);

	msg_tree = initialize_msg_avl();		// Initialize trees that will store parsed data from .dbc file
	signal_tree = initialize_signal_avl();

	char *fileName = "IOM2014.dbc";			// Your .dbc file
	parseFile(fileName);	// Parse the file

	struct canfd_frame frame;
	int s, nbytes;
	struct sockaddr_can addr;
	struct ifreq ifr;
	s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	strcpy(ifr.ifr_name, "can0" );
	ioctl(s, SIOCGIFINDEX, &ifr);
	addr.can_family = AF_CAN;
	addr.can_ifindex = 0; // Used to be ifr.ifr_ifindex
	if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		printf("Problem binding socket");
		return 1;
	}

	sem_init(&semaphore, 0, 1);
	sem_init(&can_semaphore, 0, 1);
	pthread_t txthread, logging;
	pthread_create(&txthread, NULL, txcanthread, s);
	pthread_create(&logging, NULL, logthread, NULL);


	unsigned long msgsRecv = 0;

	while(keepRunning){
		/*if(msgsRecv > 10000) // Debugging message limit
		{
			keepRunning = 0;
		}*/
		sem_wait(&can_semaphore);
		nbytes = read(s, &frame, sizeof(struct canfd_frame)); //This will stop the program until a message is received
		sem_post(&can_semaphore);

		//Don't allow the translation of bad reads
		if (nbytes < sizeof(struct can_frame))
		{
			fprintf(stderr, "read: incomplete CAN frame\n");
			return 1;
		}
		if (nbytes < 0)
		{
			perror("Close socket?");
			close(s);
			pthread_join(txthread, NULL);
			pthread_join(logging, NULL);
			delete_tree(signal_tree);
			delete_tree(msg_tree);
			return 1;
		}
		if (nbytes > 0)
		{
			//printf("%lu\n", msgsRecv);	// Debugging the receiving of messages from CAN bus.
			msgsRecv++;
			translate(msg_tree, signal_tree, &frame);
		}
	}
	close(s);
	pthread_join(txthread, NULL); // We want to close the threads before deleting the trees they are accessing
	pthread_join(logging, NULL);
	delete_tree(signal_tree);
	delete_tree(msg_tree);
	free(signal_tree);
	signal_tree = NULL;
	free(msg_tree);
	msg_tree = NULL;
	return 0;
}
