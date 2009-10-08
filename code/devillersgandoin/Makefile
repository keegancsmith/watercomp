CXX=g++
CXXFLAGS=-O3 -Wall -I../components
CXXLIBS=-lm

# Just get all .o files in components 0 to 1 level deep.
SRC  = $(wildcard ../components/*.cpp)
SRC += $(wildcard ../components/*/*.cpp)
SRC += $(wildcard ../dcd_loader/*.cpp)
OBJS = $(patsubst %.cpp, %.o, $(filter-out %/main.cpp, $(SRC)))

default: dgcomp test_transformation test_point_compression

dgcomp: dgcomp.cpp devillersgandoin.o $(OBJS)
	$(CXX) $(CXXFLAGS) $(CXXLIBS) -o dgcomp dgcomp.cpp devillersgandoin.o $(OBJS)

test_transformation: test_transformation.cpp devillersgandoin.cpp devillersgandoin.h
	$(CXX) $(CXXFLAGS) $(CXXLIBS) -DDIMENSIONS=2 -o test_transformation \
	test_transformation.cpp devillersgandoin.cpp

test_point_compression: test_point_compression.cpp devillersgandoin.cpp devillersgandoin.h
	$(CXX) $(CXXFLAGS) $(CXXLIBS) -o test_point_compression \
	test_point_compression.cpp devillersgandoin.cpp ac.c

clean:
	rm -f test_point_compression test_transformation dgcomp $(OBJS)

check-syntax:
	$(CXX) $(CXXFLAGS) -Wextra -Wno-sign-compare -fsyntax-only $(CHK_SOURCES)
