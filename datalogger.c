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
double prev_system_time;
int headers_logged = 0, renamed = 0;
struct signal_node sig_node_key;
struct signal_node * sig_node;
struct signal_node * data;
extern sem_t semaphore;
extern FILE *f;
extern char logString[15];
time_t rawtime;
struct tm * timeinfo;

void insert_headers(void *n)
{
	data = (struct signal_node *) n;
	fprintf(f, "%s", data->key);
	if(data->signal->unit != NULL)
	{
		fprintf(f, " %s,", data->signal->unit);
	}
	else
	{
		fprintf(f, ",");
	}
	gettimeofday(&start_tv, NULL);
}

void node_to_file(void *n)
{
	data = (struct signal_node *) n;
	if(data == NULL)
	{
		printf("There was a pointer that was NULL. Find me\n");
		exit(1);
	}
	fprintf(f, "%f,", data->value);
}

void data_log(tree *signal_tree)
{
	// The signal to determine if we need to rename the file and change system time
	strcpy(sig_node_key.key, "GPSValidityStatus");
	sig_node = get_signal(signal_tree, &sig_node_key, sizeof(struct signal_node));
	if(sig_node->signal == NULL || sig_node == NULL)
	{
		printf("There was a pointer that was NULL. Find me\n");
		exit(1);
	}
	if(sig_node != NULL && sig_node->value == 1)
	{
		if(renamed == 0)
		{
			printf("About to change shit");
			rename_log(signal_tree);
			renamed = 1;
		}
	}
	gettimeofday(&now_tv, NULL);
	// Prints runtime
	fprintf(f, "%g,", (double) (now_tv.tv_usec - start_tv.tv_usec) / 1000000 + (double) (now_tv.tv_sec - start_tv.tv_sec) + prev_system_time);
	// Prints system time
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	fprintf(f, "%.*s,", 24, asctime (timeinfo));

	// Semaphore to protect against reading data that is being changed
	sem_wait(&semaphore);
	explore_tree(signal_tree, node_to_file);
	sem_post(&semaphore);
	fprintf(f, "\n");
}

int rename_log(tree *signal_tree)
{
	char fileName[22] = "", value[5];
	// Obtain Y, M, D, H, M, S for tree, store in something local
	int year, month, day, hour, minute, seconds;
	strcpy(sig_node_key.key, "Years");
	sig_node = get_signal(signal_tree, &sig_node_key, sizeof(struct signal_node));
	year = sig_node->value;
	if(year > 2500) // If we time traveled to the year 2500, we would surely come in last place... we don't want that
	{
		renamed = 0;
		return 1;
	}
	sprintf(value, "%d", year);
	strcat(fileName, value);
	strcat(fileName, "-");

	strcpy(sig_node_key.key, "Month");
	sig_node = get_signal(signal_tree, &sig_node_key, sizeof(struct signal_node));
	month = sig_node->value;
	if(month > 12)
	{
		renamed = 0;
		return 1;
	}
	sprintf(value, "%d", month);
	strcat(fileName, value);
	strcat(fileName, "-");

	strcpy(sig_node_key.key, "Day");
	sig_node = get_signal(signal_tree, &sig_node_key, sizeof(struct signal_node));
	day = sig_node->value;
	if(day > 31)
	{
		renamed = 0;
		return 1;
	}
	sprintf(value, "%d", day);
	strcat(fileName, value);
	strcat(fileName, "--");

	strcpy(sig_node_key.key, "Hours");
	sig_node = get_signal(signal_tree, &sig_node_key, sizeof(struct signal_node));
	hour = sig_node->value;
	if(hour > 24)
	{
		renamed = 0;
		return 1;
	}
	sprintf(value, "%d", hour);
	strcat(fileName, value);
	strcat(fileName, "-");

	strcpy(sig_node_key.key, "Minutes");
	sig_node = get_signal(signal_tree, &sig_node_key, sizeof(struct signal_node));
	minute = sig_node->value;
	if(minute > 60)
	{
		renamed = 0;
		return 1;
	}
	sprintf(value, "%d", minute);
	strcat(fileName, value);
	strcat(fileName, ".csv");

	strcpy(sig_node_key.key, "Seconds");
	sig_node = get_signal(signal_tree, &sig_node_key, sizeof(struct signal_node));
	seconds = sig_node->value;
	if(seconds > 60)
	{
		renamed = 0;
		return 1;
	}

	new_tv.tv_sec = ((year-1970)*31556900 + (month-1)*2629740 + (day-1)*86400 + hour*3600 + minute*60 + seconds);
	prev_system_time = (double) (now_tv.tv_usec - start_tv.tv_usec) / 1000000 + (double) (now_tv.tv_sec - start_tv.tv_sec);
	settimeofday(&new_tv, NULL);
	gettimeofday(&start_tv, NULL);

	//Final fileName format = 2014-2-1--15-36 (Y-M-D--Hour-Minutes)
	fclose(f);
	// Rename file, reopen it for datalogging
	printf("%s\n", logString);
	rename(logString, fileName);
	f = fopen(fileName, "a");
	return 0;
}


