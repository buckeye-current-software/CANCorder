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

#include <sys/time.h>
#include <sys/types.h>


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

tree *msg_tree;
tree *signal_tree;

int main()
{
	msg_tree = initialize_msg_avl();
	signal_tree = initialize_signal_avl();
	time_t cur_time, prev_time;
	time(&prev_time);

	char *fileName = "test2.dbc";
	parseFile(fileName);

	FILE *f;
	f = fopen("log.txt", "w");
	if(f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	explore_tree(signal_tree, insert_headers, f); //Generates headers once (can change to once in so many lines written)
	fprintf(f, "\n"); 							  // New line after all signals generated as headers

	struct can_frame frame;
	int s, nbytes;
	struct sockaddr_can addr;
	struct ifreq ifr;
	s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	strcpy(ifr.ifr_name, "vcan0" );
	ioctl(s, SIOCGIFINDEX, &ifr);
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	bind(s, (struct sockaddr *)&addr, sizeof(addr));

	while(1){
		time(&cur_time);

		nbytes = read(s, &frame, sizeof(struct can_frame));
		printf("nbytes: %i", nbytes);
		if (nbytes < 0)
		{
			perror("can raw socket read");
			return 1;
		}
		if (nbytes > 0)
		{
			printf("About to translate\n");
			translate(msg_tree, signal_tree, &frame);
		}
		else {
			printf("Not translating. Nbytes = %d\n", nbytes);
		}

		if((int)cur_time-(int)prev_time >= 1)    // Once every second...
		{
			data_log(signal_tree, f);
			fflush(f); 						     // Perhaps we should flush once every ~5 seconds to minimize a slow down
			time(&prev_time);
		}

	}

	fclose(f);

	return 0;
}
