/*
 * datalogger.c
 *
 *  Created on: Jan 19, 2014
 *      Author: Sean
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "datalogger.h"
#include "avl.h"
#include "SignalAVL.h"
#include "avl.h"

struct timeval start_tv, now_tv, new_tv, old_tv;
double system_time;
int headers_logged = 0, renamed = 0;
struct signal_node sig_node;
extern sem_t semaphore;
extern FILE *f;
extern char logString;
time_t rawtime;
struct tm * timeinfo;

void insert_headers(void *n, void *param)
{
	struct signal_node *data = (struct signal_node *) n;
	fprintf(f, "%s,", data->key);
	gettimeofday(&start_tv, NULL);
}

void node_to_file(void *n, void *param)
{
	struct signal_node *data = (struct signal_node *) n;
	fprintf(f, "%g,", data->value);
}

void data_log(tree *signal_tree)
{
	sig_node.key = (char*)malloc(20 * sizeof(char));
	strcpy(sig_node.key, "GPS_validity_status");
	get_data(signal_tree, &sig_node, sizeof(struct signal_node));
	if(sig_node.value == 1)
	{
		if(renamed == 0)
		{
			rename_log(signal_tree);
		}
	}
	gettimeofday(&now_tv, NULL);
	// Prints runtime
	fprintf(f, "%g,", (double) (now_tv.tv_usec - start_tv.tv_usec) / 1000000 +
	         (double) ((now_tv.tv_sec-old_tv.tv_sec) - start_tv.tv_sec));
	// Prints system time
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	fprintf(f, "%.*s,", 24, asctime (timeinfo));

	sem_wait(&semaphore);
	explore_tree(signal_tree, node_to_file, f);
	sem_post(&semaphore);
	fprintf(f, "\n");
}

void rename_log(tree *signal_tree)
{
	char fileName[18] = "", value[5];
	// Obtain Y, M, D, H, M, S for tree, store in something local
	int year, month, day, hour, minute, seconds;
	strcpy(sig_node.key, "Year");
	get_data(signal_tree, &sig_node, sizeof(struct signal_node));
	year = sig_node.value;
	sprintf(value, "%d", year);
	strcat(fileName, value);
	strcat(fileName, "-");

	strcpy(sig_node.key, "Month");
	get_data(signal_tree, &sig_node, sizeof(struct signal_node));
	month = sig_node.value;
	sprintf(value, "%d", month);
	strcat(fileName, value);
	strcat(fileName, "-");

	strcpy(sig_node.key, "Day");
	get_data(signal_tree, &sig_node, sizeof(struct signal_node));
	day = sig_node.value;
	sprintf(value, "%d", day);
	strcat(fileName, value);
	strcat(fileName, "--");

	strcpy(sig_node.key, "Hour");
	get_data(signal_tree, &sig_node, sizeof(struct signal_node));
	hour = sig_node.value;
	sprintf(value, "%d", hour);
	strcat(fileName, value);
	strcat(fileName, "-");

	strcpy(sig_node.key, "Minute");
	get_data(signal_tree, &sig_node, sizeof(struct signal_node));
	minute = sig_node.value;
	sprintf(value, "%d", minute);
	strcat(fileName, value);

	//strcpy(sig_node.key, "Second");
	get_data(signal_tree, &sig_node, sizeof(struct signal_node));
	seconds = sig_node.value;
	//sprintf(value, "%d", seconds);
	//strcat(fileName, value);

	new_tv.tv_sec = year*31556900 + month*2629740 + day*86400 + hour*3600 + minute*60 + seconds;
	gettimeofday(&old_tv, NULL);
	settimeofday(&new_tv, NULL);
	//Final fileName format = 2014-2-1--15-36
	fclose(f);
	link(logString, fileName);
	unlink(logString);
	f = fopen(logString, "a");
}


