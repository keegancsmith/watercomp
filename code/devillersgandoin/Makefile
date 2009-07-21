CC=g++

CFLAGS=-Wall -g
CLIBS=-lm

devillers: main.cpp devillersgandoin.cpp devillersgandoin.h
	$(CC) $(CFLAGS) $(CLIBS) -o devillers main.cpp devillersgandoin.cpp devillersgandoin.h

check-syntax:
	$(CC) $(CFLAGS) -Wextra -Wno-sign-compare -fsyntax-only $(CHK_SOURCES)

