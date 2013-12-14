new_huffman_driver: new_huffman.o
	/s/gcc-4.8.0/bin/g++ -O2 -Wall -std=c++11 new_huffman.o new_huffman_driver.cpp -o new_huffman_driver
newvalgrind: new_huffman_driver
	valgrind --leak-check=yes new_huffman_driver < inputs/huckfinnfreqs

new_huffman.o: new_huffman.cpp new_huffman.h
	/s/gcc-4.8.0/bin/g++ -O2 -Wall -std=c++11 -c new_huffman.cpp -o new_huffman.o

all: huffman_encode

huffman.o: huffman.h huffman.cpp
	/s/gcc-4.8.0/bin/g++ -g -Wall -std=c++11 huffman.cpp -c -o huffman.o

huffman_encode: huffman_encode.cpp huffman.o
	/s/gcc-4.8.0/bin/g++ -g -Wall -std=c++11 huffman.o huffman_encode.cpp -o huffman_encode

valgrind1: huffman_encode
	valgrind --leak-check=yes huffman_encode < inputs/huckfinnfreqs
valgrind2: huffman_encode
	valgrind --leak-check=yes huffman_encode < inputs/wikifreqs
valgrind3: huffman_encode
	valgrind --leak-check=yes huffman_encode < inputs/wikipicfreqs

clean:
	rm -f *~ *.o
