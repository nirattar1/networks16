/*
 * client.c
 *
 *  Created on: Nov 22, 2016
 *      Author: nirattar
 */

#include "client.h"
#include <stdlib.h>

int main ()
{

	//create socket.
	int sfd = setup_client();


	//connect to server.
	struct sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons( DEFAULT_PORT );
	inet_pton(AF_INET, "127.0.0.1", &dest_addr.sin_addr);
	connect(sfd, (struct sockaddr*) &dest_addr, sizeof(struct sockaddr));


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


void handle_error (const char * msg)
{
	printf("%s", msg);
}
