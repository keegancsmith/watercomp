CXX=g++
CXXFLAGS=-O3 -Wall -I../components -I../ann/include -DANN
CXXLIBS=-lm -L../ann/lib -lANN

# Just get all .o files in components 0 to 1 level deep.
SRC  = $(wildcard *.cpp)
SRC += $(wildcard ../components/*.cpp)
SRC += $(wildcard ../components/*/*.cpp)
SRC += $(wildcard ../dcd_loader/*.cpp)
OBJS = $(patsubst %.cpp, %.o, $(filter-out %/main.cpp, $(SRC)))

watercomp: $(OBJS)
	$(CXX) $(CXXFLAGS) -o watercomp $(OBJS) $(CXXLIBS)

clean:
	rm -f watercomp $(OBJS)

check-syntax:
	$(CXX) $(CXXFLAGS) -Wextra -Wno-sign-compare -fsyntax-only $(CHK_SOURCES)
