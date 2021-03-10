CC = gcc
CFLAGS = -g

TARGET = HW
SRC = main.c arr.c count.c
OBJ = main.o arr.o count.o

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

.PHONY : clean
clean:
	rm $(TARGET) $(OBJ)

main.o: header.h main.c
arr.o : header.h arr.c
count.o: header.h count.c