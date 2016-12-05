CFLAGS=-Wall -g -std=gnu99 


O_FILES= server.o utils.o client.o console_ui.o msg.o msg_db.o protocol.o
EXECUTABLES= mail_client mail_server


all: $(EXECUTABLES)

clean:
	-rm $(EXECUTABLES) $(O_FILES)

mail_client: client.o protocol.o utils.o msg.o console_ui.o
	gcc -o $@ $^ $(CFLAGS)

mail_server: server.o protocol.o utils.o msg.o msg_db.o
	gcc -o $@ $^ $(CFLAGS)

client.o: client.c client.h protocol.c protocol.h utils.h
	gcc -c $(CFLAGS) $*.c

server.o: server.c server.h protocol.c protocol.h msg_db.c msg_db.h utils.h
	gcc -c $(CFLAGS) $*.c

protocol.o: protocol.c protocol.h
	gcc -c $(CFLAGS) $*.c

msg_db.o: msg.c msg.h msg_db.c msg_db.h utils.h
	gcc -c $(CFLAGS) $*.c

msg.o: msg.c msg.h protocol.c protocol.h
	gcc -c $(CFLAGS) $*.c

console_ui.o: console_ui.c console_ui.h
	gcc -c $(CFLAGS) $*.c

#
#nim-server: server.o protocol.o utils.o
#	gcc -o $@ $^ $(CFLAGS)
#
#nim: client.o nim-client.o protocol.o utils.o
#	gcc -o $@ $^ $(CFLAGS)
#
#client.o: client.c nim-client.c nim-client.h utils.c utils.h  protocol.c protocol.h  
#	gcc -c $(CFLAGS) $*.c
#
#nim-client.o: nim-client.c nim-client.h protocol.c protocol.h
#	gcc -c $(CFLAGS) $*.c
#
#protocol.o: protocol.c protocol.h
#	gcc -c $(CFLAGS) $*.c
#
#server.o: server.c server.h utils.c utils.h  protocol.c protocol.h
#	gcc -c $(CFLAGS) $*.c