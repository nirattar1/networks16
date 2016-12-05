/*
 * utils.c
 *
 *  Created on: Dec 3, 2016
 *      Author: nirattar
 */
#include "utils.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>


int sendall(int socket, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(socket, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
}


int recvall(int socket, char *buf, int *len)
{
    int total = 0;        // how many bytes we've received
    int bytesleft = *len; // how many we have left to receive
    int n;

    while(total < *len) {
        n = recv(socket, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
}


int recv_until_delim(int socket, char *buf, char delim, int *len)
{
    int total = 0;        // how many bytes we've received
    int n;
    char lastc = 0; //assume delimiter is not null char.

    //going on until delimiter or length reached.
    while(lastc!=delim && total < *len) {

    	//get char from socket.
    	n = recv(socket, &lastc, 1, 0);
        if (n == -1) { break; };
        //copying char if not delimiter.
        if (lastc!=delim)
		{
			buf[total] = lastc;
		}
        total += n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
}


//will get a char pointer and its length.
//will zero out all the chars up to length.
void ZeroCharBuf(char * buf, int length)
{
	for (int i=0; i<length; i++)
	{
		buf[i] = 0;
	}
}



void handle_error (const char * msg)
{
	printf("%s", msg);
}
