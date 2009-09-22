CXX=g++
CXXFLAGS=-O3 -Wall

all: AtomInformation.o Frame.o PDBReader.o DCDReader.o

check-syntax:
	$(CXX) $(CXXFLAGS) -Wextra -Wno-sign-compare -fsyntax-only $(CHK_SOURCES)
