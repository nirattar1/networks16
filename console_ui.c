/*
 * console_ui.c
 *
 *  Created on: Dec 5, 2016
 *      Author: nirattar
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "console_ui.h"
#include "utils.h"
#include "client.h"

static void Menu_Client_PrintError(ClientCommandStatus cmd_status);

int readline (char * line)
{
	if ( fgets (line , MAX_COMMAND_LENGTH , stdin) == NULL )
	{
			printf("No line\n");
			return 0;
	}
	else
	{
		//there is line

		//clean the last \n
		char * new_line = strchr(line, '\n');
		//replace newline, if found, with null char.
		if (new_line!=NULL)
		{
			*new_line = 0;
		}
		return 1;
	}
}



int Menu_ClientLoginReadAndSend(int socket)
{
	//buffer to store username
	char line[MAX_COMMAND_LENGTH];

	//read 1st line from user
	ZeroCharBuf(line, MAX_COMMAND_LENGTH);
	if (!readline(line))
	{
		//if readline failed, then input has closed.
		//quitting.
		exit (1);
	}
	//line should be in format
	//User: username
	char tmp_username[MAX_USERNAME];
	ZeroCharBuf(tmp_username, MAX_USERNAME);
	int result = 0;
	result = sscanf(line, "User: %s", tmp_username);
	if (result != 1)
	{
		handle_error("failed to read username.\n");
		exit (1);
	}

	//read 2nd line from user
	ZeroCharBuf(line, MAX_COMMAND_LENGTH);
	if (!readline(line))
	{
		//if readline failed, then input has closed.
		//quitting.
		exit (1);
	}
	char tmp_password[MAX_PASSWORD];
	ZeroCharBuf(tmp_password, MAX_PASSWORD);
	result = sscanf(line, "Password: %s", tmp_password);
	if (result != 1)
	{
		handle_error("failed to read password.\n");
		exit (1);
	}

	//two buffers contain what we want.
	debug_print("read username %s\n", tmp_username);
	debug_print("read password %s\n", tmp_password);

	//send them in a login request
	ProtocolRequest req;
	ProtocolRequest_Init(&req);
	req._method = METHOD_LOGIN;
	AddHeaderRequest(&req, "User", tmp_username);
	AddHeaderRequest(&req, "Password", tmp_password);

	//send request
	SendReqToSocket (socket, &req);

	//read reply from socket
	ProtocolReply rep;
	ProtocolReply_Init(&rep);
	ReadRepHeadersFromSocket(socket, &rep, req._method);


	if(rep._status==REPLY_STATUS_OK)
	{
		//we are logged in
	}
	else
	{
		handle_error("failure to login.");
		exit(1);
	}


}

int Menu_ClientReadAndDoCommands(int socket)
{
	//buffer to store commands
	char line[MAX_COMMAND_LENGTH];
	ZeroCharBuf(line, MAX_COMMAND_LENGTH);

	//while user doesn't enter QUIT
	while (1)
	{

		//read line from user
		if (!readline(line))
		{
			//if readline failed, then input has closed.
			//quitting.
			exit (1);
		}

		ClientCommandStatus cmd_status = CLIENT_COMMAND_STATUS_ILLEGAL_COMMAND;

		//read next command
		if (strncmp(line, "SHOW_INBOX", 10)==0)
		{
			//create show inbox request and send it.
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

			cmd_status = CLIENT_COMMAND_STATUS_OK;
		}


		else if (strncmp(line, "QUIT", 4)==0)
		{
			cmd_status = CLIENT_COMMAND_STATUS_OK;
			exit (1);
		}

		//handle unsuccessful commands.
		//(print the appropriate error)
		if (cmd_status!=CLIENT_COMMAND_STATUS_OK)
		{
			Menu_Client_PrintError(cmd_status);
		}

	}

	return 0;

}


static void Menu_Client_PrintError(ClientCommandStatus cmd_status)
{
	switch (cmd_status)
	{

		case CLIENT_COMMAND_STATUS_ILLEGAL_COMMAND:
			printf ("%s\n", "illegal command");
			break;
		default:
			printf ("%s\n", "illegal command");
			break;
	}


}
