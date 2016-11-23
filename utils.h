/*
 * utils.h
 *
 *  Created on: Nov 23, 2016
 *      Author: nirattar
 */

#ifndef UTILS_H_
#define UTILS_H_

#define DEBUG

#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif


#define debug_print(fmt, ...) \
            do { if (DEBUG_TEST) fprintf(stderr, fmt, __VA_ARGS__); } while (0)


#endif /* UTILS_H_ */