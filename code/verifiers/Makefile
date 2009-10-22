CXX=g++
CXXFLAGS=-O3 -Wall -I../components
CXXLIBS=-lm

# Just get all .o files in components 0 to 1 level deep.
SRC = $(wildcard ../components/*.cpp)
SRC += $(wildcard ../components/*/*.cpp)
SRC += $(wildcard ../dcd_loader/*.cpp)
OBJS = $(patsubst %.cpp, %.o, $(filter-out %/main.cpp, $(SRC)))

default: checkcomp checkdcd frameextract atomextract

checkcomp: checkcomp.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) $(CXXLIBS) -o checkcomp checkcomp.cpp $(OBJS)

checkdcd: checkdcd.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) $(CXXLIBS) -o checkdcd checkdcd.cpp $(OBJS)

frameextract: frameextract.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) $(CXXLIBS) -o frameextract frameextract.cpp $(OBJS)

atomextract: atomextract.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) $(CXXLIBS) -o atomextract atomextract.cpp $(OBJS)

clean:
	rm -f checkdcd frameextract atomextract $(OBJS)

check-syntax:
	$(CXX) $(CXXFLAGS) -Wextra -Wno-sign-compare -fsyntax-only $(CHK_SOURCES)
