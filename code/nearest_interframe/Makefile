CXX=g++
CXXFLAGS=-O3 -I../components/ 

INCS = ../dcd_loader/dcd.h ../components/pdbio/DCDReader.h ../components/pdbio/DCDWriter.h ../components/pdbio/Frame.h ../components/quantiser/QuantisedFrame.h ../components/FrameWriter.h ../components/FrameReader.h ../components/arithmetic/ArithmeticEncoder.h ../components/arithmetic/AdaptiveModelEncoder.h ../components/arithmetic/ArithmeticDecoder.h ../components/arithmetic/AdaptiveModelDecoder.h ../components/arithmetic/FenwickTree.h NearestInterframeWriter.h NearestInterframeReader.h

SOURCES = ../dcd_loader/dcd.cpp ../components/pdbio/DCDReader.cpp ../components/pdbio/DCDWriter.cpp ../components/pdbio/Frame.cpp ../components/quantiser/QuantisedFrame.cpp ../components/FrameWriter.cpp ../components/FrameReader.cpp ../components/arithmetic/ArithmeticEncoder.cpp ../components/arithmetic/AdaptiveModelEncoder.cpp ../components/arithmetic/ArithmeticDecoder.cpp ../components/arithmetic/AdaptiveModelDecoder.cpp  ../components/arithmetic/FenwickTree.cpp NearestInterframeWriter.cpp NearestInterframeReader.cpp

OBJECTS = ../dcd_loader/dcd.o ../components/pdbio/DCDReader.o ../components/pdbio/DCDWriter.o  ../components/pdbio/Frame.o ../components/quantiser/QuantisedFrame.o ../components/FrameWriter.o ../components/FrameReader.o ../components/arithmetic/ArithmeticEncoder.o ../components/arithmetic/AdaptiveModelEncoder.o ../components/arithmetic/ArithmeticDecoder.o ../components/arithmetic/AdaptiveModelDecoder.o ../components/arithmetic/FenwickTree.o  NearestInterframeWriter.o NearestInterframeReader.o


all: $(OBJECTS)
	make driver

driver: $(INCS) $(SOURCES) $(OBJECTS) main.cpp
	$(CXX) $(CXXFLAGS) $(OBJECTS) main.cpp -o driver

clean:
	rm -rf $(OBJECTS)
check-syntax:
	$(CXX) $(CXXFLAGS) -Wextra -Wno-sign-compare -fsyntax-only $(CHK_SOURCES)
