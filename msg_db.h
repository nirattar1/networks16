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

	MailMessage _msgs [MAXMAILS];


} Mail_DB;



//CTOR for mail db.
void Mail_DB_Init (Mail_DB * db);

//get a specific mail message of user.
//return a pointer to a message, null if not found.
const MailMessage * GetMail(int mail_id, const char * user_id,
		const Mail_DB * db);

//loop over DB. Out of all messages sent to user user_id,
//find the message in index "mail_id" (relative to that user).
//returns the index of that message (absolute in DB), -1 on failure.
int GetUserMailDbIndex (int mail_id, const char * user_id, const Mail_DB * db);

#endif /* MSG_DB_H_ */
