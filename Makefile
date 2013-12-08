all: huffman_encode

huffman.o: huffman.h huffman.cpp
	/s/gcc-4.8.0/bin/g++ -g -Wall -std=c++11 huffman.cpp -c -o huffman.o

huffman_encode: huffman_encode.cpp huffman.o
	/s/gcc-4.8.0/bin/g++ -g -Wall -std=c++11 huffman.o huffman_encode.cpp -o huffman_encode

valgrind: huffman
	valgrind --leak-check=yes huffman < dasguptafreqs

clean:
	rm -f huffman_encode *~ *.o
