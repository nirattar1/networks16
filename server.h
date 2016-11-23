/*
 * server.h
 *
 *  Created on: Nov 22, 2016
 *      Author: nirattar
 */

#ifndef SERVER_H_
#define SERVER_H_
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#define DEFAULT_PORT 6423

int setup_server();

void handle_connection (int socket, struct sockaddr_in * client_addr, socklen_t client_addr_len);

void handle_error (const char * msg);

#endif /* SERVER_H_ */
