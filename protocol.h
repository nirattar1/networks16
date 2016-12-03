/*
 * protocol.h
 *
 *  Created on: Dec 3, 2016
 *      Author: nirattar
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "msg.h"

//provide: 1. data structures to represent protocol requests/replies.
//2. functionality to translate between protocol requests/replies
//and mail message data structures.

//longest header name is "password"
#define MAX_HEADER_NAME_LENGTH 8
//longest header value is in mail content field ("compose" method)
#define MAX_HEADER_VALUE_LENGTH MAX_CONTENT
//there can be maximum 3 headers ("compose" method)
#define MAX_NUM_HEADERS 3

typedef enum {
	METHOD_NULL,
	METHOD_LOGIN,
	METHOD_SHOW_INBOX,
	METHOD_GET,
	METHOD_DELETE,
	METHOD_COMPOSE
} Req_Method;

typedef struct ProtocolHeader
{
	char _name [MAX_HEADER_NAME_LENGTH];
	char _value [MAX_HEADER_VALUE_LENGTH];
} ProtocolHeader;

typedef struct ProtocolRequest
{
	//methods
	Req_Method _method;
	//headers
	ProtocolHeader _headers [MAX_NUM_HEADERS];
} ProtocolRequest;


//REPLY
typedef enum {
	REPLY_STATUS_NULL,
	REPLY_STATUS_OK,
	REPLY_STATUS_UNAUTHORIZED,
	REPLY_STATUS_LOGIN_REQUIRED,
	REPLY_STATUS_GEN_ERROR
} Rep_Status;

#endif /* PROTOCOL_H_ */
