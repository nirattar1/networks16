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
UsersDB _users_db;

Mail_DB * GetServerDB()
{
	return &_server_db;
}

UsersDB * GetUsersDB()
{
	return &_users_db;
}


//"CTOR". will zero all fields.
void User_Init (User * usr)
{
	ZeroCharBuf(usr->_username, MAX_USERNAME);
	ZeroCharBuf(usr->_password, MAX_PASSWORD);
}

//"CTOR". will zero all fields.
void UsersDB_Init (UsersDB * usr_db)
{
	usr_db->_num_users = 0;
	for (int i=0; i < NUM_OF_CLIENTS; i++)
	{
		User_Init(&(usr_db->_users[i]));
	}
}

//will add user to DB.
int UsersDB_AddUser(UsersDB * db, const char * username, const char * password)
{
	if (db==NULL || username==NULL || password==NULL)
	{
		handle_error("null pointer UsersDB_AddUser");
		return 0;
	}

	//if DB full , fail.
	if (db->_num_users>=NUM_OF_CLIENTS)
	{
		handle_error("user DB is full");
		return 0;
	}

	//DB is not full. insert at next available index.
	int index = db->_num_users;
	User_Init(&(db->_users[index]));
	strcpy(db->_users[index]._password, password);
	strcpy(db->_users[index]._username, username);

   //incrmeent users count
   db->_num_users++;
   return 1;

}

int UsersDB_IsMatchLogin(const UsersDB * db, const char * username, const char * password)
{

	if (db==NULL || username==NULL || password==NULL)
	{
		handle_error("null pointer UsersDB_IsMatchLogin");
		return 0;
	}

	//iterate users on DB.
	for (int i=0; i<db->_num_users; i++)
	{
		//try match user AND password.
		if(strcmp(username, db->_users[i]._username)==0
				&& strcmp(password, db->_users[i]._password)==0)
		{
			//match!
			return 1;
		}
	}

	//if reached here then didn't found a match.
	return 0;
}

int UsersDB_ReadFromFile (UsersDB * db, const char * filename)
{
	int line_cnt = 1;
	FILE * fd = fopen (filename , "r");
	char lineBuf[MAX_USERS_FILE_LINE];

	if (fd == NULL)
	{
		printf("opening users file failed. filename: %s\n", filename);
		exit(0);
	}

	//file is now open - read one line from it.
	while (!feof(fd))
	{
		 //working line by line. (assume max line length is constant)
		 //use fgets/fscanf to read into temp buffer.

		 if (fgets ( lineBuf, MAX_USERS_FILE_LINE, fd))
		 {
			 //success, buffer has line content. (eof might have been reached)
			 debug_print ("%s\n", lineBuf ) ;
			 //parse line according to format:
			 //username\tpassword\n
			 char curr_username [MAX_USERNAME];
			 char curr_password [MAX_PASSWORD];
			 int result = sscanf(lineBuf, "%s\t%s\n", curr_username, curr_password);
			 if (result>=2)
			 {
				 //read 2 tokens (successful read.)
				 UsersDB_AddUser(db, curr_username, curr_password);
			 }
			 else
			 {
				printf("error parsing users file %s failed, line %d.",
						filename, line_cnt);
				exit(0);
			 }
		 }
		 else	//eof reached without reading bytes, or error
		 {
		 }
		 //increment line count
		 line_cnt++;
	}

	//finally close the file.
	fclose (fd);

}


int main (int argc, char* argv[])
{

	//connection parameters
	char * port_str = DEFAULT_PORT;
	char * user_db_filename = NULL;

	//read arguments. (must be 1)
	if (argc<2)
	{
		handle_error("too few arguments.\n");
		exit(1);
	}
	if (argc==2)  //argument for DB filename.
	{
		//read filename
		user_db_filename = argv[1];
		debug_print("using db filename: %s\n", user_db_filename);
	}
	else if (argc>=3) //arguments for hostname+port.
	{
		//read filename
		user_db_filename = argv[1];
		debug_print("using db filename: %s\n", user_db_filename);

		//read port.
		port_str = argv[2];
		debug_print("using port: %s\n", port_str);
	}


	//initialize user DB.
	UsersDB_Init(&_users_db);
	//read users info from file
	UsersDB_ReadFromFile (&_users_db, user_db_filename);
	debug_print("read %d users from file %s user_db_filename. \n",
			_users_db._num_users, user_db_filename);

	//initialize msgs DB.
	Mail_DB_Init(&_server_db);

	//create test data (to be removed)
	Mail_DB_CreateTestData(&_server_db);

	//create socket, bind and listen.
	int port = atoi(port_str); //get port as number
	int sfd = setup_server(port);

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
		handle_connection (conn, &client_addr, client_addr_len, &_users_db);
	}

	if (close(sfd))	//should return 0
	{
		handle_error("close error");
	}


	exit(0);
}


int setup_server(int port)
{

	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1)
	{
		handle_error("socket");
	}
	struct sockaddr_in my_addr, client_addr;

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons( port );
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



void handle_connection (int conn, struct sockaddr_in * client_addr,
		socklen_t client_addr_len, const UsersDB * users_db)
{
	//new connection
	debug_print("%s\n", "new connection.");

	//send greeting to client
	//TODO change greeting to protocol messages
	char msg_buf[GREET_MSG_BUFF_MAX_LEN];
	ZeroCharBuf(msg_buf, GREET_MSG_BUFF_MAX_LEN);
	strcpy(msg_buf, GREET_MSG);
	int len = GREET_MSG_BUFF_MAX_LEN;
	sendall(conn, msg_buf, &len);

	//perform client login.

	//read login request from client.
	ProtocolRequest req;
	ProtocolRequest_Init(&req);
	ReadReqFromSocket(conn, &req);

	//read user and password mail id from req header.
	if (strcmp(req._headers[0]._name, "User")!=0
			|| strcmp(req._headers[1]._name, "Password")!=0)
	{
		handle_error("could not get login details.");
	}

	//username
	char * curr_user = req._headers[0]._value;
	//password
	char * curr_password = req._headers[1]._value;

	//match user password against user db
	int login_result = UsersDB_IsMatchLogin(users_db, curr_user, curr_password);

	//create reply with appropriate error code.
	ProtocolReply rep;
	ProtocolReply_Init (&rep);

	if (login_result==1)
	{
		rep._status = REPLY_STATUS_OK;
		debug_print("user login %s was successful.\n", curr_user);
	}
	else
	{
		rep._status = REPLY_STATUS_GEN_ERROR;
		debug_print("error. user login failed. user: %s, password: %s\n", curr_user, curr_password);
	}

	//send the reply over socket.
	SendRepToSocket (socket, &rep);


	//
	//	char curr_user [MAX_USERNAME];
	//	ZeroCharBuf(curr_user, MAX_USERNAME);
	//	strcpy (curr_user, "Esther");
	//	char curr_password [MAX_PASSWORD];
	//	ZeroCharBuf(curr_password, MAX_PASSWORD);
	//	strcpy (curr_password, "abcddddde");




	while(1)
	{
		//read request from client.
		ProtocolRequest req;

		//test - another req
		ProtocolRequest_Init(&req);
		ReadReqFromSocket(conn, &req);

		debug_print("req type:%d\n", req._method);
		//TODO Print header nicely

		//handle user's request.
		//will also return reply.
		RequestDispatch(conn, &req, curr_user);
	}

	//close connection
	debug_print("%s\n", "closing connection.");
	if (close(conn))	//should return zero on success
	{
		handle_error("close connection failed.");
	}

}


void RequestDispatch (int socket, const ProtocolRequest * req, const char * curr_user)
{

	//GET method
	if (req->_method==METHOD_GET)
	{

		debug_print("%s\n", "handling msg get.");

		//read the mail id from header.
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

	//COMPOSE method
	else if (req->_method==METHOD_COMPOSE)
	{
		debug_print("%s\n", "handling msg compose.");

		//build a message object based on request.
		MailMessage msg;
		Message_Init(&msg);
		MsgFromRequest_Server(&msg, req, curr_user);

		//add msg to DB.
		Mail_DB * db = GetServerDB();
		int added = AddMail(&msg, db);

		//create reply with appropriate error code.
		ProtocolReply rep;
		ProtocolReply_Init (&rep);
		if (!added)
		{
			rep._status = REPLY_STATUS_GEN_ERROR;
			debug_print("%s\n", "did not add mail message.");
		}
		else
		{
			debug_print("%s\n", "added mail message.");
			rep._status = REPLY_STATUS_OK;
		}

		//send the reply over socket.
		SendRepToSocket (socket, &rep);
	}

	//SHOW_INBOX method
	else if (req->_method==METHOD_SHOW_INBOX)
	{
		debug_print("%s\n", "handling msg show inbox.");

		//create reply with appropriate error code.
		ProtocolReply rep;
		ProtocolReply_Init (&rep);

		//TODO can there be failure here?
		debug_print("%s\n", "printing mail list.");
		rep._status = REPLY_STATUS_OK;

		//send the reply over socket.
		SendRepToSocket (socket, &rep);

		const Mail_DB * db = GetServerDB();

		//find and send messages to reply.
		SendUserMailList(socket, curr_user, db);


	}

}


