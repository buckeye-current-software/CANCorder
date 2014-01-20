/*
 * main.c
 *
 *  Created on: Jan 15, 2014
 *      Author: Sean
 */
#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <time.h>
/*
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <net/if.h>

#include <linux/can.h>
#include <linux/can/raw.h>
*/
#include "parser.h"
#include "avl.h"
#include "MessageAVL.h"
#include "SignalAVL.h"
#include "datalogger.h"

tree *msg_tree;
tree *signal_tree;

int main()
{
	printf("Start of main");
	msg_tree = initialize_msg_avl();
	signal_tree = initialize_signal_avl();
	time_t cur_time, prev_time;
	time(&prev_time);

	char *fileName = "test2.dbc";
	parseFile(fileName);
	printf("out of parser");

	FILE *f;
	f = fopen("log.txt", "w");



	if(f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	explore_tree(signal_tree, insert_headers, f); //Generates headers once (can change to once in so many lines written)
	fprintf(f, "\n");
	fflush(f);

	while(1){
		time(&cur_time);

		if((int)cur_time-(int)prev_time >= 1)
		{
			data_log(signal_tree, f);
			fflush(f);
			time(&prev_time);
		}

	}

	fclose(f);




	return 0;
}
