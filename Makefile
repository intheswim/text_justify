CC = g++

CFLAGS = -std=c++11 -Wall -Wextra -O2
CLIBS = -lm

all : main.cpp justifyText compat
		$(CC) $(CFLAGS) -o justify main.cpp $(CLIBS) justifyText.o compat.o

justifyText : justifyText.cpp justifyText.h
		$(CC) $(CFLAGS) -c justifyText.cpp $(CLIBS) 

compat : compat.cpp compat.h
		$(CC) $(CFLAGS) -c compat.cpp $(CLIBS) 

.PHONY: clean

clean :
		-rm justify justifyText.o