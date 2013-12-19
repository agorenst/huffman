new_huffman_driver: new_huffman_driver.cpp new_huffman.o
	clang++ -Wall -std=c++11 new_huffman_driver.cpp new_huffman.o -o new_huffman_driver


new_huffman.o: new_huffman.cpp new_huffman.h
	clang++ -Wall -std=c++11 new_huffman.cpp -c


clean:
	rm -f *~ *.o
