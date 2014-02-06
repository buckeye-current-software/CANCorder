/*
 * datalogger.h
 *
 *  Created on: Jan 19, 2014
 *      Author: Sean
 */

#ifndef DATALOGGER_H_
#define DATALOGGER_H_

#include "avl.h"

void insert_headers(void *n, void *param);

void node_to_file(void *n, void *param);

void data_log(tree *signal_tree);

void rename_log(tree *signal_tree);

#endif /* DATALOGGER_H_ */
