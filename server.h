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
#include "protocol.h"
#include "msg_db.h"

#define DEFAULT_PORT 6423

int setup_server();

void handle_connection (int socket, struct sockaddr_in * client_addr, socklen_t client_addr_len);

//will get a request object,
//and call the correct functionality including parameters call.
//will send the reply on socket.
void RequestDispatch (int socket, const ProtocolRequest * req, const char * curr_user);


//return the current server DB.
Mail_DB * GetServerDB();

void handle_error (const char * msg);

#endif /* SERVER_H_ */
