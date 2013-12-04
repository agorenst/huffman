all: huffman

huffman: huffman.cpp
	g++ -std=c++0x -Wall huffman.cpp -o huffman

clean:
	rm -f huffman *~ *.o
