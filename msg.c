/*
 * msg.c
 *
 *  Created on: Dec 3, 2016
 *      Author: nirattar
 */

#include "utils.h"
#include "msg.h"
#include <stdlib.h>

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


void RecipientsFromText (MailMessage * msg, char * text_buffer)
{
	if (msg==NULL || text_buffer==NULL)
	{
		handle_error("null pointer RecipientsFromText");
		return;
	}

	//read recipient in tokens
	char * token_recipient = strtok (text_buffer, ",");
	while (token_recipient != NULL)
	{
		//add recipient
		int result = Message_AddRecipient (msg, token_recipient);
		if (result==0) {return;};
		token_recipient = strtok (NULL, ",");
	}
}
