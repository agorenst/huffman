all: huffman

huffman: huffman.cpp
	/s/gcc-4.8.0/bin/g++ -Wall -std=c++11 huffman.cpp -o huffman

valgrind: huffman
	valgrind --leak-check=yes huffman < dasguptafreqs

clean:
	rm -f huffman *~ *.o
