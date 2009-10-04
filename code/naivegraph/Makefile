CXX=g++
CXXFLAGS=-g -Wall -I../components
CXXLIBS=-lm

# Just get all .o files in components 0 to 1 level deep.
SRC  = $(wildcard *.cpp)
SRC += $(wildcard ../components/*.cpp)
SRC += $(wildcard ../components/*/*.cpp)
SRC += $(wildcard ../dcd_loader/*.cpp)
OBJS = $(patsubst %.cpp, %.o, $(filter-out %/main.cpp, $(SRC)))

naivegraph: naivegraph.cpp $(OBJS)
	cd ../components; $(MAKE) $(MFLAGS)
	$(CXX) $(CXXFLAGS) $(CXXLIBS) -o naivegraph $(OBJS)

clean:
	rm -f naivegraph $(OBJS)

check-syntax:
	$(CXX) $(CXXFLAGS) -Wextra -Wno-sign-compare -fsyntax-only $(CHK_SOURCES)
