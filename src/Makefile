CC:=gcc
CFLAGS:= -g -Wall -Wextra
BIN:=duplicate-ips-monitor
LIB:=/usr/lib
EXTRA:= -lubus -lubox


SRC:=$(wildcard *.c)
OBJS:=$(SRC:.c=.o)

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) -L $(LIB) -o $@ $^ $(EXTRA)

%.o: %.c
	$(CC) $(CFLAGS) -I. -c $< -o $@

clean:
	rm -f $(BIN) $(OBJS)
