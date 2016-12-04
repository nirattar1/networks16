/*
 * server.c
 *
 *  Created on: Nov 22, 2016
 *      Author: nirattar
 */

#include "server.h"
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "protocol.h"
#include "msg_db.h"
#include <stdio.h>


//GLOBALS
Mail_DB _server_db;


Mail_DB * GetServerDB()
{
	return &_server_db;
}

int main ()
{

	//initialize msgs DB.
	Mail_DB_Init(&_server_db);

	//create test data (to be removed)
	Mail_DB_CreateTestData(&_server_db);

	//create socket, bind and listen.
	int sfd = setup_server();

	struct sockaddr_in client_addr;
	socklen_t client_addr_len;

	int keep_accepting = 1;

	while (keep_accepting)
	{

		//try accepting a new client
		client_addr_len = sizeof (struct sockaddr_in);
		int conn = accept(sfd, &client_addr, &client_addr_len);

		//handle failure
		if (!conn)
		{
			keep_accepting = 0;
		}

		//connection is ok, handle it.
		handle_connection (conn, &client_addr, client_addr_len);
	}

	if (close(sfd))	//should return 0
	{
		handle_error("close error");
	}


	exit(0);
}


int setup_server()
{

	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1)
	{
		handle_error("socket");
	}
	struct sockaddr_in my_addr, client_addr;

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons( DEFAULT_PORT );
	inet_pton(AF_INET, "127.0.0.1", &my_addr.sin_addr);

	//bind
	bind(sfd, &my_addr, sizeof(my_addr));
	//ERROR CODE

	//listen
	listen(sfd, 5);
	//ERROR CODE

	debug_print("%s\n", "listening to clients...");

	return sfd;

}



void handle_connection (int conn, struct sockaddr_in * client_addr, socklen_t client_addr_len)
{
	//new connection
	debug_print("%s\n", "new connection.");

	//TODO change to protocol messages
	//send greeting to client
	char msg_buf[GREET_MSG_BUFF_MAX_LEN];
	ZeroCharBuf(msg_buf, GREET_MSG_BUFF_MAX_LEN);
	strcpy(msg_buf, GREET_MSG);
	int len = GREET_MSG_BUFF_MAX_LEN;
	sendall(conn, msg_buf, &len);

	//perform client login.
	char curr_user [MAX_USERNAME];
	ZeroCharBuf(curr_user, MAX_USERNAME);
	strcpy (curr_user, "Moshe");

	//read request from client.
	ProtocolRequest req;

	//test - another req
	ProtocolRequest_Init(&req);
	ReadReqFromSocket(conn, &req);

	debug_print("req type:%d\n", req._method);
	debug_print("req header 1 name: %s", req._headers[0]._name);
	debug_print("req header 1 val: %s", req._headers[0]._value);
	debug_print("req header 2 name: %s", req._headers[1]._name);
	debug_print("req header 2 val: %s", req._headers[1]._value);

	//handle user's request.
	//will also return reply.
	RequestDispatch(conn, &req, curr_user);


	//close connection
	debug_print("%s\n", "closing connection.");
	if (close(conn))	//should return zero on success
	{
		handle_error("close connection failed.");
	}

}


void RequestDispatch (int socket, const ProtocolRequest * req, const char * curr_user)
{
	int expected_num_headers = GetExpectedNumHeaders_ForReq(req->_method);

	//GET method
	if (req->_method==METHOD_GET)
	{
		//we need to read the mail id from header.

		if (strcmp(req->_headers[0]._name, "mail_id")!=0)
		{
			handle_error("could not get mail id field");
		}

		int mail_id = atoi(req->_headers[0]._value);

		//get the correct mail of the user.
		const Mail_DB * db = GetServerDB();
		const MailMessage * found_mail = GetMail(mail_id, curr_user, db);

		//create reply from found message.
		ProtocolReply rep;
		ProtocolReply_Init (&rep);
		if (found_mail==NULL)
		{
			rep._status = REPLY_STATUS_GEN_ERROR;
			debug_print("%s\n", "did not find mail message.");
		}
		else
		{
			debug_print("%s\n", "found mail message.");
			rep._status = REPLY_STATUS_OK;
			MsgToReply(found_mail, &rep);
		}
		//send the reply over socket.
		SendRepToSocket (socket, &rep);
	}
}


