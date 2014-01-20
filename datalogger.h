/*
 * datalogger.h
 *
 *  Created on: Jan 19, 2014
 *      Author: Sean
 */

#ifndef DATALOGGER_H_
#define DATALOGGER_H_

void insert_headers(void *n, void *param);

void node_to_file(void *n, void *param);

void data_log(tree *signal_tree, FILE *f);

#endif /* DATALOGGER_H_ */
