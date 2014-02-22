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
#include <libgen.h>
#include <time.h>
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
sem_t semaphore, free_semaphore, malloc_semaphore, can_semaphore;
FILE *f;
char logString[15];

int keepRunning = 1;

void my_handler(int dummy) {
    keepRunning = 0;
}

void *txcanthread(int cansock) {

	struct can_frame txmsg;
	// New line after all signals generated as headers
	while(1)
	{
		sleep(1);
		txmsg.can_id  = 0xAA;
		txmsg.can_dlc = 0;
		sem_wait(&can_semaphore);
		write(cansock, &txmsg, sizeof(struct can_frame));
		sem_post(&can_semaphore);
	}
	return NULL;
}

void *logthread()
{
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

	f = fopen(logString, "w");
	if(f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}
	fprintf(f, "Runtime,System_Time,");
	explore_tree(signal_tree, insert_headers, f); //Generates headers once (can change to once in so many lines written)
	fprintf(f, "\n");
	while(1)
	{
		usleep(25000);
		data_log(signal_tree);
		fflush(f);
		//printf("Logged\n");
	}
	fclose(f);
	return NULL;
}

int main()
{
	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = my_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);


	msg_tree = initialize_msg_avl();
	signal_tree = initialize_signal_avl();

	char *fileName = "IOM2014.dbc";
	parseFile(fileName);

	struct can_frame frame;
	int s, nbytes;
	struct sockaddr_can addr;
	struct ifreq ifr;
	s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	strcpy(ifr.ifr_name, "can0" );
	ioctl(s, SIOCGIFINDEX, &ifr);
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	bind(s, (struct sockaddr *)&addr, sizeof(addr));

	sem_init(&semaphore, 0, 1);
	sem_init(&malloc_semaphore, 0, 1);
	sem_init(&free_semaphore, 0, 1);
	sem_init(&can_semaphore, 0, 1);
	pthread_t txthread, logging;
	pthread_create(&txthread, NULL, txcanthread, s);
	pthread_create(&logging, NULL, logthread, NULL);


	unsigned long msgsRecv = 0;

	while(keepRunning){
		sem_wait(&can_semaphore);
		nbytes = read(s, &frame, sizeof(struct can_frame)); //This will flat out stop the program until a message is received
		sem_post(&can_semaphore);
		//printf("nbytes: %i", nbytes);
		if (nbytes < 0)
		{
			perror("can raw socket read");
			close(s);
			pthread_join(txthread, NULL);
			pthread_join(logging, NULL);
			delete_tree(signal_tree);
			delete_tree(msg_tree);
			return 1;
		}
		if (nbytes > 0)
		{
			printf("%lu\n", msgsRecv);
			msgsRecv++;
			translate(msg_tree, signal_tree, &frame);
		}
	}
	close(s);
	pthread_join(txthread, NULL);
	pthread_join(logging, NULL);
	delete_tree(signal_tree);
	delete_tree(msg_tree);
	return 0;
}
