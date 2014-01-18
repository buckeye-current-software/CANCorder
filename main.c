/*
 * main.c
 *
 *  Created on: Jan 15, 2014
 *      Author: Sean
 */
#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>

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


int main()
{


	char *fileName = "test2.dbc";
	parseFile(fileName);

	// Translator stuff here
	return 0;
}
