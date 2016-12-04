/*
 * client.c
 *
 *  Created on: Nov 22, 2016
 *      Author: nirattar
 */

#include "client.h"
#include "protocol.h"
#include <stdlib.h>
#include <string.h>


int main ()
{


	//client connect to server right away.

	//create socket.
	int sfd = setup_client();

	//connect to server.
	struct sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons( DEFAULT_PORT );
	inet_pton(AF_INET, "127.0.0.1", &dest_addr.sin_addr);
	connect(sfd, (struct sockaddr*) &dest_addr, sizeof(struct sockaddr));
	//ERROR CODE

	//connection is ready. handle according to protocol/input.

	//handle according to protocol.
	handle_connection (sfd);


	//end of transaction with server. close socket.
	if (close(sfd))	//should return 0
	{
		handle_error("close error");
	}



	exit(0);
}


int setup_client()
{


	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1)
	{
		handle_error("socket");
	}

	return sfd;

}



void handle_connection (int socket)
{

	//first read greeting from client.
	char msg_buf[GREET_MSG_BUFF_MAX_LEN];
	recv(socket, msg_buf, GREET_MSG_BUFF_MAX_LEN, 0);

	//display greeting message
	printf("%s\n", msg_buf);


	//send test request.
//	ProtocolRequest req;
//	ProtocolRequest_Init(&req);
//	req._method = METHOD_COMPOSE;
//	sprintf (req._headers[0]._name, "To");
//	sprintf (req._headers[0]._value, "bla");
//	SendReqToSocket (socket, &req);

	//send test request.
	//("GET" request)
	ProtocolRequest req;
	ProtocolRequest_Init(&req);
	req._method = METHOD_GET;
	sprintf (req._headers[0]._name, "mail_id");
	sprintf (req._headers[0]._value, "1");
	SendReqToSocket (socket, &req);

	//read reply from socket
	ProtocolReply rep;
	ProtocolReply_Init(&rep);
	ReadRepFromSocket(socket, &rep, req._method);

	//handle reply (based on request)
	ReplyHandle(&rep, req._method);


}


void ReplyHandle(ProtocolReply * rep, Req_Method method)
{

	//handle the reply based on the original request
	if (method==METHOD_GET)
	{
		//user asked for a mail message.

		if(rep->_status==REPLY_STATUS_OK)
		{
			//construct msg object from reply.
			MailMessage msg;
			Message_Init(&msg);
			MsgFromReply(&msg, rep);

			//handle message (print it)
			debug_print("msg content:%s\n", msg._content);
			debug_print("msg from:%s\n", msg._from);
			debug_print("msg to:%s\n", msg._to[0]);
			debug_print("msg subject:%s\n", msg._subject);
		}
		else
		{
			handle_error("failure get method.");
		}
	}
}

