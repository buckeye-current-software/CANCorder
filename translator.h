/*
 * translator.h
 *
 *  Created on: Jan 18, 2014
 *      Author: Sean
 */

#ifndef TRANSLATOR_H_
#define TRANSLATOR_H_

#include "avl.h"

void translate(tree *message_tree, tree *signal_tree, struct can_frame *frame);

#endif /* TRANSLATOR_H_ */
