/*
 * msg.c
 *
 *  Created on: Dec 3, 2016
 *      Author: nirattar
 */

#include "utils.h"
#include "msg.h"
#include "protocol.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void Message_Init (MailMessage * msg)
{
	ZeroCharBuf(msg->_from, MAX_USERNAME);
	for (int i=0; i<TOTAL_TO; i++)
	{
		ZeroCharBuf(msg->_to[i], MAX_USERNAME);
	}

	msg->_num_recipients = 0;

	ZeroCharBuf(msg->_subject, MAX_SUBJECT);
	ZeroCharBuf(msg->_content, MAX_CONTENT);

}


void Message_Copy (MailMessage * dest, const MailMessage * src)
{
	if (dest==NULL || src==NULL)
	{
		handle_error("null pointer Message_Copy");
	}

	//copy 'from' field
	strncpy(dest->_from, src->_from, MAX_USERNAME);

	//copy recipients
	dest->_num_recipients = src->_num_recipients;
	for (int i=0; i<src->_num_recipients; i++)
	{
		//copy 'to' field
		strncpy(dest->_to[i], src->_to[i], MAX_USERNAME);
	}

	//copy subject
	strncpy(dest->_subject, src->_subject, MAX_SUBJECT);

	//copy content
	strncpy(dest->_content, src->_content, MAX_CONTENT);
}


int Message_MatchesRecipient (const MailMessage * msg, const char * recipient)
{
	if (msg==NULL || recipient==NULL)
	{
		handle_error("null pointer Message_MatchesRecipient");
		return 0;
	}

	//iterate on all of message's recipients
	for (int i=0; i<msg->_num_recipients; i++)
	{
		//compare against current recipient
		if (strcmp(msg->_to[i], recipient)==0)
		{
			//found user!
			return 1;
		}
	}
	return 0;
}

int Message_AddRecipient (MailMessage * msg, const char * recipient)
{

	if (msg==NULL || recipient==NULL)
	{
		handle_error("null pointer Message_AddRecipient");
		return 0;
	}

	//check if message can store recipient
	if (msg->_num_recipients >= TOTAL_TO)
	{
		debug_print("%s\n", "message exceeds num of recipients.");
		return 0;
	}

	//add user name as recipient
	int indexRecipient = msg->_num_recipients;
	strcpy(msg->_to[indexRecipient], recipient);

	//increment recipients
	msg->_num_recipients++;

	return 1;
}


void RecipientsToText (const MailMessage * msg, char * text_buffer)
{
	if (msg==NULL || text_buffer==NULL)
	{
		handle_error("null pointer RecipientsToText");
		return;
	}

	for (int i=0; i<msg->_num_recipients; i++)
	{
		//insert leading comma ',' after first.
		if (i>0)
		{
			strcat(text_buffer, ",");
		}
		//insert name
		strcat(text_buffer, msg->_to[i]);
	}
}


void RecipientsFromText (MailMessage * msg, const char * text_buffer)
{
	if (msg==NULL || text_buffer==NULL)
	{
		handle_error("null pointer RecipientsFromText");
		return;
	}

	debug_print("buffer to tokenize: %s\n", text_buffer);

	//copy to local buffer to use strtok
	char buff_tmp[MAX_HEADER_VALUE_LENGTH];//TODO don't be dependent in protocol
	ZeroCharBuf(buff_tmp, MAX_HEADER_VALUE_LENGTH);
	strcpy(buff_tmp, text_buffer);

	//read recipient in tokens
	char * token_recipient;
	token_recipient = strtok (buff_tmp, ",");
	while (token_recipient != NULL)
	{
		//add recipient
		debug_print("token: %s\n", token_recipient);
		int result = Message_AddRecipient (msg, token_recipient);
		if (result==0) {return;};
		token_recipient = strtok (NULL, ",");
	}
}

int MsgToDescription(int index, const MailMessage * msg, char * buf_description)
{
	int n = 0;	//number of bytes written

	//format of line: 'mail_id sender "subject"\n'
	n += sprintf(buf_description, "%d %s \"%s\"\n", index, msg->_from, msg->_subject);

	return n;
}
