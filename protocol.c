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



//"CTOR"
void ProtocolHeader_Init (ProtocolHeader * header)
{
	ZeroCharBuf(header->_name, MAX_HEADER_NAME_LENGTH);
	ZeroCharBuf(header->_value, MAX_HEADER_VALUE_LENGTH);
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


void ReadReqFromSocket (int socket, ProtocolRequest * req)
{
	//read method.
	char buff_method[LEN_METHOD_NAME];
	int len = LEN_METHOD_NAME;
	int n = recvall(socket, buff_method, &len);
	req->_method = StringToMethod(buff_method);

	//read newline
	char c;
	recv(socket, &c, 1, 0);

	//expect to see req headers based on method.
	int expected_num_headers = 0;
	switch (req->_method)
	{
		case METHOD_LOGIN: expected_num_headers = 2; break;
		case METHOD_SHOW_INBOX: expected_num_headers = 0; break;
		case METHOD_GET: expected_num_headers = 1; break;
		case METHOD_DELETE: expected_num_headers = 1; break;
		case METHOD_COMPOSE: expected_num_headers = 3; break;
	}

	//read req headers.
	for (int i=0; i<expected_num_headers; i++)
	{
		//read header name
		int len = MAX_HEADER_NAME_LENGTH;
		recv_until_delim(socket, req->_headers[i]._name, ':', &len);

		//read header value
		len = MAX_HEADER_VALUE_LENGTH;
		recv_until_delim(socket, req->_headers[i]._value, '\n', &len);
	}


}

//will get a reply struct, and a connection socket.
//will send the appropriate reply to the socket.
void SendRepToSocket (int socket, const ProtocolReply * rep)
{
	;
}