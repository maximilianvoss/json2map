# C Compiler
CC=llvm-gcc
CFLAGS=-Wall -ggdb -g -O2 -fstack-protector-strong -Wformat -Werror=format-security -D_FORTIFY_SOURCE=2 -DLINUX -D_REENTRANT -D_GNU_SOURCE -pthread -DDEBUG -c

# Sources
CSOURCES=$(wildcard *.c)
OBJECTS=$(CSOURCES:.c=.o)
EXECUTABLE=example

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLE)
	rm -f $(CSOURCES:.c=.o)