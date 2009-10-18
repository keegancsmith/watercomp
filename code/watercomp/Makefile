CXX=g++
CXXFLAGS=-O3 -Wall -I../components
CXXLIBS=-lm

# Just get all .o files in components 0 to 1 level deep.
SRC  = $(wildcard *.cpp)
SRC += $(wildcard ../components/*.cpp)
SRC += $(wildcard ../components/*/*.cpp)
SRC += $(wildcard ../dcd_loader/*.cpp)
OBJS = $(patsubst %.cpp, %.o, $(filter-out %/main.cpp, $(SRC)))

watercomp: watercomp.o $(OBJS)
	$(CXX) $(CXXFLAGS) $(CXXLIBS) -o watercomp $(OBJS)

clean:
	rm -f watercomp $(OBJS)

check-syntax:
	$(CXX) $(CXXFLAGS) -Wextra -Wno-sign-compare -fsyntax-only $(CHK_SOURCES)
