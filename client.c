/*
 * client.c
 *
 *  Created on: Nov 22, 2016
 *      Author: nirattar
 */


#include "client.h"
#include "protocol.h"
#include "console_ui.h"
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>


static void Test_SendTestReqs(int socket);
static void Test_ShowInbox(int socket);

int main(int argc, char* argv[])
{

	char * port_str = DEFAULT_PORT;
	char default_hostname [] = DEFAULT_HOSTNAME;
	char * hostname_or_ip = default_hostname;

	//read arguments. (on no arguments - defaults will be used).
	if (argc==2)  //argument for hostname.
	{
		//read host/ipname
		hostname_or_ip = argv[1];
		debug_print("using hostname: %s\n", hostname_or_ip);
	}
	else if (argc>=3) //arguments for hostname+port.
	{
		//read host/ipname
		hostname_or_ip = argv[1];
		debug_print("using hostname: %s\n", hostname_or_ip);

		//read port.
		port_str = argv[2];
		debug_print("using port: %s\n", port_str);
	}


	//client will create socket and connect to server right away.
	int sfd = setup_client(port_str, hostname_or_ip);

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


int setup_client(const char * port_str, const char * hostname_str)
{

	int sfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;

	//get address info (including DNS)
	if ((rv = getaddrinfo(hostname_str, port_str, &hints, &servinfo)) != 0) {
	    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
	    exit(1);
	}

	//act based on results of address info.
	//(connect to the first available result)
	for(p = servinfo; p != NULL; p = p->ai_next)
	{
	    if ((sfd = socket(p->ai_family, p->ai_socktype,
	            p->ai_protocol)) == -1)
	    {
	        perror("socket");
	        continue;
	    }

	    if (connect(sfd, p->ai_addr, p->ai_addrlen) == -1)
	    {
	        perror("connect");
	        close(sfd);
	        continue;
	    }

	    break; // if we get here, we must have connected successfully
	}

	if (p == NULL) {
	    // looped off the end of the list with no connection
	    fprintf(stderr, "failed to connect\n");
	    exit(2);
	}


	return sfd;

}



void handle_connection (int socket)
{

	//first read greeting from client.
	char msg_buf[GREET_MSG_BUFF_MAX_LEN];
	ZeroCharBuf(msg_buf, GREET_MSG_BUFF_MAX_LEN);
	//TODO replace to protocol message
	int len = GREET_MSG_BUFF_MAX_LEN;
	recvall(socket, msg_buf, &len);

	//display greeting message
	printf("%s\n", msg_buf);

	//perform login (exit on wrong login)
	Menu_ClientLoginReadAndSend(socket);

	//if reached here, then login was successful.
	//read and performs loop of commands from user.
	//will send to socket.
	Menu_ClientReadAndDoCommands(socket);

	//tests
	//Test_ShowInbox(socket);
	//Test_SendTestReqs(socket);
}


static void Test_ShowInbox(int socket)
{
	//send test request.
	//("SHOW_INBOX" request)
	ProtocolRequest req;
	ProtocolRequest_Init(&req);
	req._method = METHOD_SHOW_INBOX;

	//send request
	SendReqToSocket (socket, &req);

	//read reply from socket
	ProtocolReply rep;
	ProtocolReply_Init(&rep);
	ReadRepHeadersFromSocket(socket, &rep, req._method);

	//handle reply (based on request)
	ReplyHandle(socket, &rep, req._method);
}

static void Test_SendTestReqs(int socket)
{
	//send test request.
	//("GET" request)
	ProtocolRequest req;
	ProtocolRequest_Init(&req);
	req._method = METHOD_GET;
	AddHeaderRequest(&req, "mail_id", "1");
	SendReqToSocket (socket, &req);

	//read reply from socket
	ProtocolReply rep;
	ProtocolReply_Init(&rep);
	ReadRepHeadersFromSocket(socket, &rep, req._method);

	//handle reply (based on request)
	ReplyHandle(socket, &rep, req._method);



	//send test request.
	//("COMPOSE" request)
	MailMessage msg;
	Message_Init(&msg);
	strcpy(msg._subject,"I was sent!");
	strcpy(msg._content,"wow this is great...");
	Message_AddRecipient (&msg, "Yaniv");//TODO check return
	Message_AddRecipient (&msg, "Naaman");
	Message_AddRecipient (&msg, "Yorai");
	//make request from message
	ProtocolRequest_Init(&req);
	req._method = METHOD_COMPOSE;
	MsgToRequest(&msg, &req);

	//send request
	SendReqToSocket (socket, &req);

	//read reply from socket
	ProtocolReply_Init(&rep);
	ReadRepHeadersFromSocket(socket, &rep, req._method);

	//handle reply (based on request)
	ReplyHandle(socket, &rep, req._method);


	//send test request.
	//("SHOW_INBOX" request)
	ProtocolRequest_Init(&req);
	req._method = METHOD_SHOW_INBOX;

	//send request
	SendReqToSocket (socket, &req);

	//read reply from socket
	ProtocolReply_Init(&rep);
	ReadRepHeadersFromSocket(socket, &rep, req._method);

	//handle reply (based on request)
	ReplyHandle(socket, &rep, req._method);
}

void ReplyHandle(int socket, ProtocolReply * rep, Req_Method method)
{

	debug_print("%s\n", "handling reply.");


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

	else if (method==METHOD_COMPOSE)
	{
		if(rep->_status==REPLY_STATUS_OK)
		{
			;
		}
		else
		{
			handle_error("failure compose method.");
		}

	}

	else if (method==METHOD_SHOW_INBOX)
	{
		if(rep->_status==REPLY_STATUS_OK)
		{
			//in this case there is still data to be read from buffer.
			//(reply content)
			//read and immediately print it.
			//reading will be done in buffers.
			ReadPrintRepContentFromSocket(socket);
		}
		else
		{
			handle_error("failure show_inbox method.");
		}

	}


}


