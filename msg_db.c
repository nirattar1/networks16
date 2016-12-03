/*
 * msg_db.c
 *
 *  Created on: Dec 3, 2016
 *      Author: nirattar
 */

#include "msg.h"
#include "msg_db.h"
#include <stdlib.h>

void Mail_DB_Init (Mail_DB * db)
{
	for (int i=0; i < MAXMAILS; i++)
	{
		Message_Init(&(db->_msgs[i]));
	}
}

const MailMessage * GetMail(int mail_id, const char * user_id,
		const Mail_DB * db)
{

	//find mail id'd "mail_id" of user "user_id"
	int db_index = GetUserMailDbIndex (mail_id, user_id, db);

	const MailMessage * found_msg = NULL;
	if (db_index!=-1)
	{
		found_msg = &(db->_msgs[db_index]);
	}

	return found_msg;
}


int GetUserMailDbIndex (int mail_id, const char * user_id, const Mail_DB * db)
{
	int user_i = 0;

	if (mail_id<1 || mail_id>MAXMAILS)	//can be equal to MAXMAILS since 1-based
	{
		return -1;
	}

	//loop over all DB.
	for (int i=0; i<MAXMAILS; i++)
	{
		//check if mail matches user
		//TODO check from all recipients
		if (strcmp(db->_msgs[i]._to, user_id))
		{
			user_i++;
		}

		if (user_i == mail_id)
		{
			return i;
		}
	}

	//if here means we haven't found appropriate msg of that user.
	return -1;
}
