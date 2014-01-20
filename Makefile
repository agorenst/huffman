driver: driver.cpp huffman.o
	clang++ -Wall -g -std=c++11 driver.cpp huffman.o -o huffman


huffman.o: huffman.cpp huffman.h
	clang++ -Wall -g -std=c++11 huffman.cpp -c


clean:
	rm -f *~ *.o huffman
