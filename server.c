/*
 * server.c
 *
 *  Created on: Nov 22, 2016
 *      Author: nirattar
 */

#include "server.h"
#include <stdlib.h>
#include "utils.h"


int main ()
{

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

	//send greeting to client
	char msg_buf[GREET_MSG_BUFF_MAX_LEN];
	strcpy(msg_buf, GREET_MSG);

	send(conn, msg_buf, GREET_MSG_BUFF_MAX_LEN, 0);

	//close connection
	debug_print("%s\n", "closing connection.");
	if (close(conn))	//should return zero on success
	{
		handle_error("close connection failed.");
	}

}

void handle_error (const char * msg)
{
	printf("%s", msg);
}
