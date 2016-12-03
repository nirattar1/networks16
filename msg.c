/*
 * msg.c
 *
 *  Created on: Dec 3, 2016
 *      Author: nirattar
 */

#include "utils.h"
#include "msg.h"

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
