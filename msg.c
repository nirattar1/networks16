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
	ZeroCharBuf(msg->_subject, MAX_SUBJECT);
	ZeroCharBuf(msg->_content, MAX_CONTENT);

}


void Message_Copy (MailMessage * dest, const MailMessage * src)
{
	if (dest==NULL || src==NULL)
	{
		handle_error("null pointer");
	}

	//copy 'from' field
	strncpy(dest->_from, src->_from, MAX_USERNAME);

	//TODO copy more 1 recipient
	//copy 'to' field
	strncpy(dest->_to[0], src->_to[0], MAX_USERNAME);

	//copy subject
	strncpy(dest->_subject, src->_subject, MAX_SUBJECT);

	//copy content
	strncpy(dest->_content, src->_content, MAX_CONTENT);
}
