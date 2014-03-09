
CC = clang
EXE = socket-handler

CFILES = connection_test.c connection.c simple_map.c
CFILES2 = connection_test_server.c connection.c data.c simple_map.c
OBJECTS = $(CFILES:.c=.o)

all:
	$(CC) $(CFILES2) -o $(EXE)

debug:
	$(CC) -g $(CFILES) -o $(EXE) -D DEBUG


