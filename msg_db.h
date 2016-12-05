/*
 * msg_db.h
 *
 *  Created on: Dec 3, 2016
 *      Author: nirattar
 */

#ifndef MSG_DB_H_
#define MSG_DB_H_

#include "msg.h"

#define MAXMAILS 32000


typedef struct Mail_DB
{
	//a buffer that stores all messages on server.
	MailMessage _msgs [MAXMAILS];

	//will hold the number of messages currently on server.
	//deleted messages are also included in this count.
	int _curr_size;

} Mail_DB;


//CTOR for mail db.
void Mail_DB_Init (Mail_DB * db);

//for test
void Mail_DB_CreateTestData(Mail_DB * db);


//get a specific mail message of user.
//return a pointer to a message, null if not found.
const MailMessage * GetMail(int mail_id, const char * user_id,
		const Mail_DB * db);

//will get a mail message and copy it to the database.
//will increment the database logical size .
//returns - 1 if succeded, 0 if failed (may be due to full database)
int AddMail(const MailMessage * msg, Mail_DB * db);

//loop over DB. Out of all messages sent to user user_id,
//find the message in index "mail_id" (relative to that user).
//returns the index of that message (absolute in DB), -1 on failure.
int GetUserMailDbIndex (int mail_id, const char * user_id, const Mail_DB * db);


//will receive a message db, a socket and a user id.
//will print to socket (line-by-line) descriptions of messages of that user.
//format of line: 'mail_id sender "subject"\n'
void SendUserMailList(int socket, const char * user_id, const Mail_DB * db);

#endif /* MSG_DB_H_ */
