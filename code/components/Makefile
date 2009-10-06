CXX=g++
CXXFLAGS=-O3 -Wall

all: FrameWriter.o FrameReader.o Compressor.o
	cd arithmetic; $(MAKE) $(MFLAGS)
	cd graph; $(MAKE) $(MFLAGS)
	cd pdbio; $(MAKE) $(MFLAGS)
	cd quantiser; $(MAKE) $(MFLAGS)

clean:
	rm -f *.o */*.o */*/*.o

check-syntax:
	$(CXX) $(CXXFLAGS) -Wextra -Wno-sign-compare -fsyntax-only $(CHK_SOURCES)
