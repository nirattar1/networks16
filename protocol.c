/*
 * protocol.c
 *
 *  Created on: Dec 3, 2016
 *      Author: nirattar
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "protocol.h"
#include <sys/socket.h>


void ProtocolRequest_Init (ProtocolRequest * req)
{
	//init method.
	req->_method = METHOD_NULL;
	for (int i=0; i<MAX_NUM_HEADERS; i++)
	{
		//init all headers.
		ProtocolHeader_Init(&(req->_headers[i]));
	}

}

void ProtocolReply_Init (ProtocolReply * rep)
{
	//init status.
	rep->_status = REPLY_STATUS_NULL;

	for (int i=0; i<MAX_NUM_HEADERS; i++)
	{
		//init all headers.
		ProtocolHeader_Init(&(rep->_headers[i]));
	}
	rep->_num_headers = 0;


	//init content.
	//TODO
}



//"CTOR"
void ProtocolHeader_Init (ProtocolHeader * header)
{
	ZeroCharBuf(header->_name, MAX_HEADER_NAME_LENGTH);
	ZeroCharBuf(header->_value, MAX_HEADER_VALUE_LENGTH);
}


//functionality to add a header to reply. (based on value)
void AddHeaderReply (ProtocolReply * rep, const char * header_name,
		const char * header_value)
{
	//fill the new header on reply.
	int next_header = rep->_num_headers;
	strcpy(rep->_headers[next_header]._name, header_name);
	strcpy(rep->_headers[next_header]._value, header_value);

	//increment number of headers on reply.
	rep->_num_headers++;
}

//will take a reply and return its status in number.
//return -1 if status or request was not defined.
int RepStatusToNum (const ProtocolReply * rep)
{
	if (rep==NULL) {return -1;};
	switch (rep->_status)
	{
	case REPLY_STATUS_OK:
		return 200;
	case REPLY_STATUS_UNAUTHORIZED:
		return 401;
	case REPLY_STATUS_LOGIN_REQUIRED:
		return 999;
	case REPLY_STATUS_GEN_ERROR:
		return 500;
	default:
		return -1;
	}
}

Rep_Status NumToRepStatus (int status)
{
	switch (status)
	{
	case 200:
		return REPLY_STATUS_OK;
	case 401:
		return REPLY_STATUS_UNAUTHORIZED;
	case 999:
		return REPLY_STATUS_LOGIN_REQUIRED;
	case 500:
		return REPLY_STATUS_GEN_ERROR;
	default:
		return REPLY_STATUS_NULL;
	}
}

//will take a method enum and a buffer, will write to buffer textual name of method.
void MethodToString (Req_Method method, char * buff_method)
{
	//clear buffer.
	ZeroCharBuf(buff_method, LEN_METHOD_NAME);

	//write appropriate word to buffer.
	switch (method)
	{
		case METHOD_LOGIN:
			sprintf(buff_method, "LGN");
			break;
		case METHOD_SHOW_INBOX:
			sprintf(buff_method, "SHW");
			break;
		case METHOD_GET:
			sprintf(buff_method, "GET");
			break;
		case METHOD_DELETE:
			sprintf(buff_method, "DEL");
			break;
		case METHOD_COMPOSE:
			sprintf(buff_method, "CMP");
			break;
		default:
			break;

	}
}

Req_Method StringToMethod(const char * buff_method)
{
	if (strcmp(buff_method, "LGN")==0) { return METHOD_LOGIN;};
	if (strcmp(buff_method, "SHW")==0) { return METHOD_SHOW_INBOX;};
	if (strcmp(buff_method, "GET")==0) { return METHOD_GET;};
	if (strcmp(buff_method, "DEL")==0) { return METHOD_DELETE;};
	if (strcmp(buff_method, "CMP")==0) { return METHOD_COMPOSE;};

	return METHOD_NULL;
}



//will take a header struct and a buffer,
//will write to buffer textual header.
int HeaderToString (const ProtocolHeader * header, char * buff_header)
{
	int num_wrote = 0;
	//copy name
	int name_len = strlen(header->_name);
	strncpy(buff_header, header->_name, name_len);
	num_wrote += name_len;

	//add ': '
	buff_header[num_wrote] = ':';
	num_wrote++;
	buff_header[num_wrote] = ' ';
	num_wrote++;

	//copy value
	int value_len = strlen(header->_value);
	strncpy(buff_header+num_wrote, header->_value, value_len);
	num_wrote+= value_len;

	return num_wrote;
}


//will get a request struct, and a connection socket.
//will send the appropriate request on the socket.
void SendReqToSocket (int socket, const ProtocolRequest * req)
{
	if (socket==0 || req==NULL)
	{
		handle_error("send error");
	}

	//send req. method
	char buff_method [LEN_METHOD_NAME+1];
	MethodToString (req->_method, buff_method);
	//insert newline
	int len = LEN_METHOD_NAME;
	buff_method[len] = '\n';
	len++;
	debug_print("%s\n", buff_method);
	sendall(socket, buff_method, &len);

	//send request headers.
	//loop over all headers and send them
	for (int i=0; i<MAX_NUM_HEADERS; i++)
	{
		const ProtocolHeader * header = &((req->_headers)[i]);
		char buff_header [MAX_HEADER_BUFF_LENGTH+1];
		int header_len = HeaderToString (header, buff_header);
		//insert newline
		buff_header[header_len] = '\n';
		header_len++;
		buff_header[header_len] = 0;
		debug_print("%s\n", buff_header);
		sendall(socket, buff_header, &header_len);

	}

}

#define STATUS_STRING_MAX_LENGTH 3

//will take a reply and send it over socket.
void SendRepToSocket (int socket, const ProtocolReply * rep)
{
	if (socket==0 || rep==NULL)
	{
		handle_error("send error");
	}

	//send reply status (number)
	//TODO support status string
	char buff_status [STATUS_STRING_MAX_LENGTH + 1];
	int status_num = RepStatusToNum (rep);
	sprintf(buff_status, "%d", status_num);

	//insert newline
	int len = STATUS_STRING_MAX_LENGTH;	//TODO support status string
	buff_status[len] = '\n';
	len++;
	debug_print("sending: %s\n", buff_status);
	sendall(socket, buff_status, &len);

	//send request headers.
	//loop over all headers and send them
	for (int i=0; i<rep->_num_headers; i++)
	{
		const ProtocolHeader * header = &((rep->_headers)[i]);
		char buff_header [MAX_HEADER_BUFF_LENGTH+1];
		int header_len = HeaderToString (header, buff_header);
		//insert newline
		buff_header[header_len] = '\n';
		header_len++;
		buff_header[header_len] = 0;
		debug_print("%s\n", buff_header);
		sendall(socket, buff_header, &header_len);

	}

}

int GetExpectedNumHeaders_ForReq(Req_Method reqMethod)
{
	int expected_num_headers = 0;
	switch (reqMethod)
	{
		case METHOD_LOGIN:
			expected_num_headers = 2;
			break;
		case METHOD_SHOW_INBOX:
			expected_num_headers = 0;
			break;
		case METHOD_GET:
			expected_num_headers = 1;
			break;
		case METHOD_DELETE:
			expected_num_headers = 1;
			break;
		case METHOD_COMPOSE:
			expected_num_headers = 3;
			break;
		default:
			expected_num_headers = 0;
		}
	return expected_num_headers;
}

//TODO check!
int GetExpectedNumHeaders_ForRep(Req_Method reqMethod)
{
	int expected_num_headers = 0;
	switch (reqMethod)
	{
		case METHOD_LOGIN:
			expected_num_headers = 2;
			break;
		case METHOD_SHOW_INBOX:
			expected_num_headers = 0;
			break;
		case METHOD_GET:
			expected_num_headers = 4;
			break;
		case METHOD_DELETE:
			expected_num_headers = 1;
			break;
		case METHOD_COMPOSE:
			expected_num_headers = 3;
			break;
		default:
			expected_num_headers = 0;
		}
	return expected_num_headers;
}

void ReadReqFromSocket (int socket, ProtocolRequest * req)
{
	//read method.
	char buff_method[LEN_METHOD_NAME];
	int len = LEN_METHOD_NAME;
	int n = recvall(socket, buff_method, &len);
	req->_method = StringToMethod(buff_method);

	//read newline
	char c_buff;
	recv(socket, &c_buff, 1, 0);

	//expect to see req headers based on method.
	int expected_num_headers = GetExpectedNumHeaders_ForReq(req->_method);

	//read req headers.
	for (int i=0; i<expected_num_headers; i++)
	{
		//read header name
		int len = MAX_HEADER_NAME_LENGTH;
		recv_until_delim(socket, req->_headers[i]._name, ':', &len);

		//read space
		len = 1;
		recv_until_delim(socket, &c_buff, ' ', &len);

		//read header value
		len = MAX_HEADER_VALUE_LENGTH;
		recv_until_delim(socket, req->_headers[i]._value, '\n', &len);
	}

}



void ReadRepFromSocket (int socket, ProtocolReply * rep, Req_Method reqMethod)
{

	if (socket==0 || rep==NULL)
	{
		handle_error("receive reply error");
	}

	//read reply status (number). assuming 3 digit number.
	//TODO support status string
	char buff_status[STATUS_STRING_MAX_LENGTH];
	int len = STATUS_STRING_MAX_LENGTH;
	int n = recvall(socket, buff_status, &len);
	int status_num=0;
	sscanf(buff_status, "%d", &status_num);
	rep->_status = NumToRepStatus(status_num);

	//read newline
	char c_buff;
	len = 1;
	recv_until_delim(socket, &c_buff, '\n', &len);

	//expect to see req headers based on request's method.
	int expected_num_headers = GetExpectedNumHeaders_ForRep(reqMethod);

	//read req headers.
	for (int i=0; i<expected_num_headers; i++)
	{
		//read header name
		len = MAX_HEADER_NAME_LENGTH;
		char buff_header_name[MAX_HEADER_NAME_LENGTH];
		recv_until_delim(socket, buff_header_name, ':', &len);

		//read space
		len = 1;
		recv_until_delim(socket, &c_buff, ' ', &len);

		//read header value
		len = MAX_HEADER_VALUE_LENGTH;
		char buff_header_value[MAX_HEADER_VALUE_LENGTH];
		recv_until_delim(socket, buff_header_value, '\n', &len);

		//add header to reply object.
		AddHeaderReply(rep, buff_header_name, buff_header_value);
	}

	//now reply object has data.
}


void MsgToReply(const MailMessage * msg, ProtocolReply * rep)
{
	//assuming valid, status was already turned to OK.

	//fill all headers.
	AddHeaderReply (rep, "From", msg->_from);
	//TODO support multi recipients
	AddHeaderReply (rep, "To", msg->_to[0]);
	AddHeaderReply (rep, "Subject", msg->_subject);
	AddHeaderReply (rep, "Text", msg->_content);
}
