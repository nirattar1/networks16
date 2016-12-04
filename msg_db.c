/*
 * msg_db.c
 *
 *  Created on: Dec 3, 2016
 *      Author: nirattar
 */

#include "msg.h"
#include "msg_db.h"
#include <stdlib.h>
#include <string.h>

void Mail_DB_Init (Mail_DB * db)
{
	for (int i=0; i < MAXMAILS; i++)
	{
		Message_Init(&(db->_msgs[i]));
	}

	db->_curr_size = 0;
}

void Mail_DB_CreateTestData(Mail_DB * db)
{
	MailMessage msg1;
	Message_Init (&msg1);
	strcpy (msg1._from, "Ayelet");
	strcpy (msg1._to[0], "Nir");
	strcpy (msg1._to[1], "Limon");
	strcpy (msg1._subject, "I am subject");
	strcpy (msg1._content, "Hi! example content");
	AddMail(&msg1, db);


	MailMessage msg2;
	Message_Init (&msg2);
	strcpy (msg2._from, "Nir");
	strcpy (msg2._to[0], "Moshe");
	strcpy (msg2._subject, "subject 2");
	strcpy (msg2._content, "blaaaaaaaa content 2");
	AddMail(&msg2, db);
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

void AddMail(const MailMessage * msg, Mail_DB * db)
{
	//go to the storage in the next available count.
	int available_index = db->_curr_size;

	//copy info
	Message_Copy(&(db->_msgs[available_index]), msg);

	//increment msg count
	db->_curr_size++;

}


int GetUserMailDbIndex (int mail_id, const char * user_id, const Mail_DB * db)
{
	int user_i = 0;
	if (db==NULL)
	{
		handle_error("null pointer - GetUserMailDbIndex");
		return -1;
	}

	if (mail_id<1 || mail_id>db->_curr_size)	//can be equal to _curr_size since 1-based
	{
		return -1;
	}

	//loop over all DB.
	for (int i=0; i<db->_curr_size; i++)
	{
		//check if mail matches user
		//TODO check from all recipients
		if (strcmp(db->_msgs[i]._to[0], user_id)==0)
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
