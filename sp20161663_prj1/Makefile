CC = gcc
CFLAGS = -g

TARGET = 20161663.out
SRC = 20161663.c command.c queue.c
OBJ = 20161663.o command.o queue.o

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

.PHONY : clean
clean:
	rm $(TARGET) $(OBJ)

20161663.o: header.h command.h queue.h 20161663.c
queue.o: header.h queue.h queue.c
command.o: header.h queue.h command.h queue.c command.c 
