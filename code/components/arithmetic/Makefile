CC = g++ -O3 -g -fno-strict-aliasing -U_FORTIFY_SOURCE -D_FILE_OFFSET_BITS=64

driver: main.cpp AdaptiveModelEncoder.cpp AdaptiveModelEncoder.h AdaptiveModelDecoder.cpp AdaptiveModelDecoder.h ArithmeticEncoder.cpp ArithmeticEncoder.h ArithmeticDecoder.cpp ArithmeticDecoder.h FenwickTree.cpp FenwickTree.h
	$(CC) main.cpp AdaptiveModelEncoder.cpp AdaptiveModelDecoder.cpp ArithmeticEncoder.cpp ArithmeticDecoder.cpp FenwickTree.cpp -o driver

clean:
	rm -f *.o