/*
 * server.h
 *
 *  Created on: Nov 22, 2016
 *      Author: nirattar
 */

#ifndef SERVER_H_
#define SERVER_H_
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include "protocol.h"
#include "msg.h"
#include "msg_db.h"


#define NUM_OF_CLIENTS 20
//length of line in user file (allow username, password, tab and \n)
#define MAX_USERS_FILE_LINE (MAX_USERNAME+MAX_PASSWORD+2)

//a struct for users DB.
typedef struct User
{
	char _username [MAX_USERNAME];
	char _password [MAX_PASSWORD];
} User;

typedef struct UsersDB
{
	User _users [NUM_OF_CLIENTS];
	int _num_users;
} UsersDB;


//"CTOR". will zero all fields.
void User_Init (User * usr);
//"CTOR". will zero all fields.
void UsersDB_Init (UsersDB * msg);
//read data into users DB, from file in given location
int UsersDB_ReadFromFile (UsersDB * db, const char * filename);
//will add user to DB.
//return 1 on success, 0 failure.
int UsersDB_AddUser(UsersDB * db, const char * username, const char * password);
//will match a given username and password against the DB.
//return 1 on success, 0 failure.
int UsersDB_IsMatchLogin(const UsersDB * db, const char * username, const char * password);



//create socket, bind and listen on port
int setup_server(int port);

void handle_connection (int socket, struct sockaddr_in * client_addr,
		socklen_t client_addr_len, const UsersDB * users_db);

//will get a request object,
//and call the correct functionality including parameters call.
//will send the reply on socket.
void RequestDispatch (int socket, const ProtocolRequest * req, const char * curr_user);


//return the current server DB.
Mail_DB * GetServerDB();

void handle_error (const char * msg);

#endif /* SERVER_H_ */
