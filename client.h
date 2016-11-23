/*
 * client.h
 *
 *  Created on: Nov 22, 2016
 *      Author: nirattar
 */

#ifndef CLIENT_H_
#define CLIENT_H_
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include "utils.h"

#define DEFAULT_PORT 6423

int setup_client();
void handle_error (const char * msg);


void handle_connection (int socket);



#endif /* CLIENT_H_ */
