driver: driver.cpp huffman.o
	clang++ -Wall -std=c++11 driver.cpp huffman.o -o driver


huffman.o: huffman.cpp huffman.h
	clang++ -Wall -std=c++11 huffman.cpp -c


clean:
	rm -f *~ *.o
