CC = gcc
CFLAGS = -Wall -g -pthread
LDFLAGS = -pthread

TARGET = buffer_sim

SRCS = main.c

all: program

program: main.o
	$(CC) $(CFLAGS) $^ -o $(TARGET)

main.o: src/main.c src/CDLL.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) *.o
