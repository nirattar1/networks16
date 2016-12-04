/*
 * utils.h
 *
 *  Created on: Nov 23, 2016
 *      Author: nirattar
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>


#define DEBUG

#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif


#define debug_print(fmt, ...) \
            do { if (DEBUG_TEST) fprintf(stderr, fmt, __VA_ARGS__); } while (0)


//will get a char pointer and its length.
//will zero out all the chars up to length.
void ZeroCharBuf(char * buf, int length);


int sendall(int socket, char *buf, int *len);

int recvall(int socket, char *buf, int *len);

//read from socket to buffer until delimiter char was read,
//or until max_len chars were read.
int recv_until_delim(int socket, char *buf, char delim, int *max_len);


void handle_error (const char * msg);


#define GREET_MSG_BUFF_MAX_LEN 1000
#define GREET_MSG "hello! welcome to mail server."

#endif /* UTILS_H_ */
