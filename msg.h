/*
 * msg.h
 *
 *  Created on: Dec 2, 2016
 *      Author: nirattar
 */

#ifndef MSG_H_
#define MSG_H_

//protocol/server limits

#define TOTAL_TO 20 //maximum recipents
#define MAX_USERNAME 60
#define MAX_PASSWORD 60
#define MAX_SUBJECT 110
#define MAX_CONTENT 2010

//provide data structures and functionality for a mail message,
//to be handled by client or server.
//both data and internal logic



typedef struct MailMessage {

	//sender username
	char _from[MAX_USERNAME];

	//recipient usernames.
	//since more than one is possible, we store them in a 2-d array.
	//there can be maximum of TOTAL_TO recipients.
	char _to[TOTAL_TO][MAX_USERNAME];

	//
	int _num_recipients;

	//subject
	char _subject[MAX_SUBJECT];

	//mail content
	char _content[MAX_CONTENT];

} MailMessage;

//"CTOR" for message struct. will zero all fields.
void Message_Init (MailMessage * msg);

//copy functionality between messages.
void Message_Copy (MailMessage * to, const MailMessage * from);

//will add new recipient to message.
//returns: 1 on success. 0 if fail (if exceed maximum)
int Message_AddRecipient (MailMessage * msg, const char * recipient);

//gets username and message.
//will return 1 if username is one of the message's recipients.
//0 otherwise.
int Message_MatchesRecipient (const MailMessage * msg, const char * recipient);

//will write to text_buffer, a list of the messages recipients.
//assume buffer is already allocated for enough space.
void RecipientsToText (const MailMessage * msg, char * text_buffer);

//read messages recipients list from text into message.
//recipients are divide by commas: "Moshe,David,..."
void RecipientsFromText (MailMessage * msg, const char * text_buffer);


//will get a message (with an index number).
//will fill buffer with description line of message.
//format of line: 'mail_id sender "subject"\n'
//return number of bytes written.
int MsgToDescription(int index, const MailMessage * msg, char * buf_description);

#endif /* MSG_H_ */
