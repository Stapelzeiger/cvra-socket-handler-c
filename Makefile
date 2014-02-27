
CC = clang
EXE = socket-handler

CFILES = connection_test.c connection.c simple_map.c
OBJECTS = $(CFILES:.c=.o)

all:
	$(CC) $(CFILES) -o $(EXE)

debug:
	$(CC) -g $(CFILES) -o $(EXE) -D DEBUG

