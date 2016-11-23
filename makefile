CFLAGS=-Wall -g -std=c99 

#O_FILES= server.o utils.o nim-client.o protocol.o client.o
O_FILES= server.o utils.o client.o
EXECUTABLES= client server


all: $(EXECUTABLES)

clean:
	-rm $(EXECUTABLES) $(O_FILES)

client: client.o
	gcc -o $@ $^ $(CFLAGS)

server: server.o
	gcc -o $@ $^ $(CFLAGS)


client.o: client.c client.h utils.h
	gcc -c $(CFLAGS) $*.c


server.o: server.c server.h utils.h
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