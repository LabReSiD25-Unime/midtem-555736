CC = gcc
CFLAGS = -g -Wall
TARGET = programma
OBJS = operazioni.o server_tcp.o 

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

check:

	@echo "\n=== Simboli file2.o ==="
	nm server_tcp.o
	@echo "\n=== Simboli main.o ==="
	nm operazioni.o
	
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean check
