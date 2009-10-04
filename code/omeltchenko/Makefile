CXX=g++
CXXFLAGS=-O3 -g -I../components/ 

INCS = ../dcd_loader/dcd.h ../components/pdbio/DCDReader.h ../components/pdbio/DCDWriter.h ../components/pdbio/Frame.h ../components/quantiser/QuantisedFrame.h SortedQuantisedFrame.h ../components/FrameWriter.h ../components/FrameReader.h OmelEncoder.h OmelDecoder.h OmelReader.h OmelWriter.h

SOURCES = ../dcd_loader/dcd.cpp ../components/pdbio/DCDReader.cpp ../components/pdbio/DCDWriter.cpp ../components/pdbio/Frame.cpp ../components/quantiser/QuantisedFrame.cpp SortedQuantisedFrame.cpp ../components/FrameWriter.cpp ../components/FrameReader.cpp OmelEncoder.cpp OmelDecoder.cpp OmelReader.cpp OmelWriter.cpp

OBJECTS = ../dcd_loader/dcd.o ../components/pdbio/DCDReader.o ../components/pdbio/DCDWriter.o  ../components/pdbio/Frame.o ../components/quantiser/QuantisedFrame.o SortedQuantisedFrame.o ../components/FrameWriter.o ../components/FrameReader.o OmelEncoder.o OmelDecoder.o OmelReader.o OmelWriter.o  

all: $(OBJECTS)
	make driver

driver: $(SOURCES) $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) main.cpp -o driver

clean:
	rm -rf $(OBJECTS)
check-syntax:
	$(CXX) $(CXXFLAGS) -Wextra -Wno-sign-compare -fsyntax-only $(CHK_SOURCES)
