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
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include "utils.h"
#include "protocol.h"



//will create socket and connect to server with given parameters.
int setup_client(const char * port_str, const char * hostname_str);


void handle_connection (int socket);

//functionality to handle with reply.
//this method may also continue to consume socket for reply content.
void ReplyHandle(int socket, ProtocolReply * rep, Req_Method method);


#endif /* CLIENT_H_ */
