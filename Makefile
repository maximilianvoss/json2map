# C Compiler
CC=gcc
CFLAGS=-Wall -g -O2 -fstack-protector-strong -Wformat -Werror=format-security -D_FORTIFY_SOURCE=2 -DLINUX -D_REENTRANT -D_GNU_SOURCE -pthread -c -D JSMN_PARENT_LINKS

# Sources
CSOURCES=$(wildcard *.c)
OBJECTS=$(CSOURCES:.c=.o)
EXECUTABLE=jsonparser

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLE)
	rm -f $(CSOURCES:.c=.o)