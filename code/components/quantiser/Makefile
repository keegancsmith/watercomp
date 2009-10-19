CXX=g++
CXXFLAGS=-O3 -Wall

all: QuantisedFrame.o Quantiser.o


check-syntax:
	$(CXX) $(CXXFLAGS) -Wextra -Wno-sign-compare -fsyntax-only $(CHK_SOURCES)
