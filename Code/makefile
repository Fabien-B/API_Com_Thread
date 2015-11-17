OBJS = abo_handeling.o gestionnaire.o init_end_service.o msg_manager.o main.o gest_abo.o
CC =  gcc
BIN = prog
CFLAGS = -W -Wall -pthread
#pourquoi -pthread et pas -lpthread ?
all:$(OBJS)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJS)

abo_handeling.o: abo_handeling.c types_private.h api_com.h
	$(CC) $(CFLAGS) -c abo_handeling.c
gestionnaire.o: gestionnaire.c types_private.h api_com.h
	$(CC) $(CFLAGS) -c gestionnaire.c
gest_abo.o: gest_abo.c types_private.h api_com.h
	$(CC) $(CFLAGS) -c gest_abo.c
init_end_service.o: init_end_service.c types_private.h api_com.h
	$(CC) $(CFLAGS) -c init_end_service.c
msg_manager.o: msg_manager.c types_private.h api_com.h
	$(CC) $(CFLAGS) -c msg_manager.c
main.o: main.c api_com.h
	$(CC) $(CFLAGS) -c main.c

clean: 
	rm -f  $(BIN) $(OBJS)

