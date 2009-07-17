CC = g++ -O3 -g -fno-strict-aliasing 

driver: main.cpp Atom.cpp Atom.h OmelReader.cpp OmelReader.h OmelWriter.cpp OmelWriter.h OmelEncoder.cpp OmelEncoder.h OmelDecoder.cpp OmelDecoder.h
	$(CC) main.cpp Atom.cpp OmelReader.cpp OmelWriter.cpp OmelEncoder.cpp OmelDecoder.cpp -o driver 
