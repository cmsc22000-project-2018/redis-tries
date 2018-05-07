/*
 * utils.h
 *
 *  Created on: Apr 1, 2018
 *      Author: borja
 */

#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <stdio.h>

#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

#define error(msg, args...) fprintf(stderr, "%s::%s::%d " msg "\n", __FILE__, __func__, __LINE__, ##args)

#endif /* SRC_UTILS_H_ */
