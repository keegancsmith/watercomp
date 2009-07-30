CC=g++

CFLAGS=-Wall -g
CLIBS=-lm


default: test_transformation test_point_compression


test_transformation: test_transformation.cpp devillersgandoin.cpp devillersgandoin.h
	$(CC) $(CFLAGS) $(CLIBS) -DDIMENSIONS=2 -o test_transformation \
	test_transformation.cpp devillersgandoin.cpp devillersgandoin.h


test_point_compression: test_point_compression.cpp devillersgandoin.cpp devillersgandoin.h
	$(CC) $(CFLAGS) $(CLIBS) -o test_point_compression \
	test_point_compression.cpp devillersgandoin.cpp devillersgandoin.h


check-syntax:
	$(CC) $(CFLAGS) -Wextra -Wno-sign-compare -fsyntax-only $(CHK_SOURCES)

