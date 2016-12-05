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
#include "utils.h"
#include "protocol.h"

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
	Message_AddRecipient(&msg1, "Nir");
	Message_AddRecipient(&msg1, "Limon");
	strcpy (msg1._subject, "I am subject");
	strcpy (msg1._content, "Hi! example content");
	AddMail(&msg1, db);


	MailMessage msg2;
	Message_Init (&msg2);
	strcpy (msg2._from, "Nir");
	Message_AddRecipient(&msg2, "Moshe");
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

int AddMail(const MailMessage * msg, Mail_DB * db)
{
	//check null ptrs.
	if (msg==NULL || db==NULL)
	{
		debug_print("%s\n", "null ptr in AddMail.");
		return 0;
	}

	//go to the storage in the next available count.
	int available_index = db->_curr_size;

	//check if already reached full capacity of db.
	if (available_index>=MAXMAILS)
	{
		return 0;
	}
	else
	{
		//enough space on db.

		//copy info
		Message_Copy(&(db->_msgs[available_index]), msg);

		//increment msg count
		db->_curr_size++;

		return 1;
	}

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
		const MailMessage * curr_msg = &(db->_msgs[i]);

		if (Message_MatchesRecipient(curr_msg, user_id)==1)
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


void SendUserMailList(int socket, const char * user_id, const Mail_DB * db)
{

	if (socket==0 || user_id==NULL || db==NULL)
	{
		handle_error("null pointer or socket - SendUserMailList");
	}

	int user_i = 0; //index of messages relative to user

	//loop over DB, find next message of user.
	for (int i=0; i<db->_curr_size; i++)
	{

		const MailMessage * curr_msg = &(db->_msgs[i]);

		//check if mail matches user AND not deleted.
		if (Message_MatchesRecipient(curr_msg, user_id)==1)
		{

			//always incremented (even if deleted)
			user_i++;

			//TODO check if deleted.
			if (1)
			{
				//prepare buffer for line.
				char msg_description_buf[MAX_MESSAGE_DESC_LINE_LEN];

				//fill buffer with message data.
				int len = MsgToDescription(user_i, curr_msg, msg_description_buf);

				//send message line.
				sendall(socket, msg_description_buf, &len);
			}
		}
	}

	//finally send last \n
	int len = 1;
	sendall(socket, "\n", &len);
}
