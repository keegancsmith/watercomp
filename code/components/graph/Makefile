CXX=g++
CXXFLAGS=-O3 -Wall

all: GraphCreator.o SpanningTree.o Graph.o TreeSerialiser.o

check-syntax:
	$(CXX) $(CXXFLAGS) -Wextra -Wno-sign-compare -fsyntax-only $(CHK_SOURCES)
