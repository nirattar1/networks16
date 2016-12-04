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

//all method names are 3 letters. (LGN, GET, ...)
#define LEN_METHOD_NAME 3
//longest header name is "password"
#define MAX_HEADER_NAME_LENGTH 8
//longest header value is in mail content field ("compose" method)
#define MAX_HEADER_VALUE_LENGTH MAX_CONTENT
//maximum length of header when sent (name, value, ' :' and '\n')
#define MAX_HEADER_BUFF_LENGTH (MAX_HEADER_NAME_LENGTH + MAX_HEADER_VALUE_LENGTH + 3)
//there can be maximum 4 headers in request OR reply ("get" method reply)
#define MAX_NUM_HEADERS 4


typedef struct ProtocolHeader
{
	char _name [MAX_HEADER_NAME_LENGTH];
	char _value [MAX_HEADER_VALUE_LENGTH];
} ProtocolHeader;

//"CTOR"
void ProtocolHeader_Init (ProtocolHeader * header);


//REQUEST
typedef enum {
	METHOD_NULL,
	METHOD_LOGIN,
	METHOD_SHOW_INBOX,
	METHOD_GET,
	METHOD_DELETE,
	METHOD_COMPOSE
} Req_Method;


//will take a method enum and a buffer, will write to buffer textual name of method.
void MethodToString (Req_Method method, char * buff_method);
Req_Method StringToMethod(const char * buff_method);



typedef struct ProtocolRequest
{
	//methods
	Req_Method _method;
	//headers
	ProtocolHeader _headers [MAX_NUM_HEADERS];
	//number of current headers.
	int _num_headers;
} ProtocolRequest;

//"CTOR"
void ProtocolRequest_Init (ProtocolRequest * req);


//REPLY
typedef enum {
	REPLY_STATUS_NULL,
	REPLY_STATUS_OK,
	REPLY_STATUS_UNAUTHORIZED,
	REPLY_STATUS_LOGIN_REQUIRED,
	REPLY_STATUS_GEN_ERROR
} Rep_Status;


typedef struct ProtocolReply
{
	//reply status code
	Rep_Status _status;
	//reply headers
	ProtocolHeader _headers [MAX_NUM_HEADERS];
	//number of current headers.
	int _num_headers;
	//reply content
} ProtocolReply;

void ProtocolReply_Init (ProtocolReply * rep);

//functionality to add a header to request. (based on value)
void AddHeaderRequest (ProtocolRequest * req, const char * header_name,
		const char * header_value);
//functionality to add a header to reply. (based on value)
void AddHeaderReply (ProtocolReply * rep, const char * header_name,
		const char * header_value);

//determine how many headers will be in REQUEST, based on req. method
int GetExpectedNumHeaders_ForReq(Req_Method reqMethod);
//determine how many headers will be in REPLY, based on req. method
int GetExpectedNumHeaders_ForRep(Req_Method reqMethod);

//will take a reply and return its status in number.
//return -1 if status or request was not defined.
int RepStatusToNum (const ProtocolReply * rep);

//gets a message and copies its data to reply object
//(used in "GET" method)
void MsgToReply(const MailMessage * msg, ProtocolReply * rep);

//build mail message object based on reply object
//(used in "GET" method)
void MsgFromReply(MailMessage * msg, const ProtocolReply * rep);


//build mail message object from request.
//(used by server for "compose")
//the "From" field of the message will be based on current user parameter.
void MsgFromRequest_Server(MailMessage * msg, const ProtocolRequest * req,
		const char * curr_user);


//will get a request struct, and a connection socket.
//will send the appropriate request on the socket.
//for client send
void SendReqToSocket (int socket, const ProtocolRequest * req);

//fill request struct based on data read from socket.
//for server receive
void ReadReqFromSocket (int socket, ProtocolRequest * req);

//will get a reply struct, and a connection socket.
//will send the appropriate reply to the socket.
//for server send
void SendRepToSocket (int socket, const ProtocolReply * rep);

//read reply from socket. (for client receive)
//will also receive the method of the originating request.
void ReadRepFromSocket (int socket, ProtocolReply * rep, Req_Method reqMethod);


#endif /* PROTOCOL_H_ */
