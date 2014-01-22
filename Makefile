driver: driver.cpp huffman.o huffman_buffers.o
	clang++ -Wall -g -std=c++11 driver.cpp huffman.o huffman_buffers.o -o huffman


huffman.o: huffman.cpp huffman.h
	clang++ -Wall -g -std=c++11 huffman.cpp -c

huffman_buffers.o: huffman_buffers.cpp huffman_buffers.h
	clang++ -Wall -g -std=c++11 huffman_buffers.cpp -c


clean:
	rm -f *~ *.o huffman

test:
	./huffman -e < MARBLES.BMP > marbles_encode 
	./huffman -c marbles_encode < MARBLES.BMP > marbles_compressed
	./huffman -d marbles_encode < marbles_compressed > marbles_expanded
