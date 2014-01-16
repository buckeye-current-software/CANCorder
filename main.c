/*
 * main.c
 *
 *  Created on: Jan 15, 2014
 *      Author: Sean
 */
#include "parser.h"

int main()
{
	char *fileName = "test2.dbc";
	parseFile(fileName);

	// delete_tree(msg_tree);
	// delete_tree(signal_tree);
	return 0;
}
