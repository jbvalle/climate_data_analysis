#Compiler
CC=gcc

#Compiler Flags
CFLAGS=-g -Wall -Wextra -pedantic -O0

#C-Files
SRCS:=$(wildcard *.c)

BINS=$(SRCS:%.c=%)

TARGET=Projekt2021_XX

all: clean $(TARGET) run


$(TARGET): $(SRCS)
	@$(CC) $(CFLAGS) -o $@ $^

run:
	./$(TARGET)

debug: $(SRCS)
	ulimit -c unlimited
	gcc $(CFLAGS) -o $(TARGET) $^ 
	gdb $(TARGET) core

memcheck: $(SRCS)
	ulimit -c unlimited
	gcc $(CFLAGS) -o $(TARGET) $^ 
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)
clean:
	rm -rf $(BINS) *.txt .*swo .*swp .*swn
