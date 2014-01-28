/*
 * datalogger.c
 *
 *  Created on: Jan 19, 2014
 *      Author: Sean
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "avl.h"
#include "SignalAVL.h"

int headers_logged = 0;

void insert_headers(void *n, void *param)
{
	struct signal_node *data = (struct signal_node *) n;
	FILE *f = (FILE *)param;
	fprintf(f, "%s,", data->key);
}

void node_to_file(void *n, void *param)
{
	struct signal_node *data = (struct signal_node *) n;
	FILE *f = (FILE *)param;
	fprintf(f, "%g,", data->value);
}

void data_log(tree *signal_tree, FILE *f)
{
	explore_tree(signal_tree, node_to_file, f);
	fprintf(f, "\n");
}


