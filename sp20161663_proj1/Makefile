CC = gcc
CFLAGS = -g

TARGET = 20161663.out
SRC = 20161663.c command.c queue.c optable.c memory.c
OBJ = 20161663.o command.o queue.o optable.o memory.o

$(TARGET): $(OBJ)
	$(CC) -Wall $(CFLAGS) -o $(TARGET) $(SRC)

.PHONY : clean
clean:
	rm $(TARGET) $(OBJ)

20161663.o: 20161663.h command.h queue.h optable.h 20161663.c
queue.o: 20161663.h queue.h queue.c
optable.o: 20161663.h optable.h optable.c
memory.o: 20161663.h memory.h memory.c
command.o: 20161663.h queue.h command.h queue.c command.c
