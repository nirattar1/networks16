/*
 * console_ui.h
 *
 *  Created on: Dec 5, 2016
 *      Author: nirattar
 */

#ifndef CONSOLE_UI_H_
#define CONSOLE_UI_H_

#include "protocol.h"

#define MAX_COMMAND_LENGTH MAX_HEADER_BUFF_LENGTH

typedef enum {
	CLIENT_COMMAND_STATUS_OK,
	CLIENT_COMMAND_STATUS_ILLEGAL_COMMAND
} ClientCommandStatus ;

//read a line from stdin, into line buffer.
//will null-terminate buffer.
int readline (char * line);

//deal with users commands.
//assumes user already performed TCP connection and login.
int Menu_ClientReadAndDoCommands(int socket);

//will read client's login details,
//then perform login to server.
int Menu_ClientLoginReadAndSend(int socket);

#endif /* CONSOLE_UI_H_ */
