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
	ZeroCharArr(msg->_from, MAX_USERNAME);
	for (int i=0; i<TOTAL_TO; i++)
	{
		ZeroCharArr(msg->_to[i], MAX_USERNAME);
	}
	ZeroCharArr(msg->_subject, MAX_SUBJECT);
	ZeroCharArr(msg->_content, MAX_CONTENT);

}
