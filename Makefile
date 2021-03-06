driver: driver.cpp huffman.o huffman_buffers.o
	g++ -Wall -g -std=c++0x driver.cpp huffman.o huffman_buffers.o -o huffman


huffman.o: huffman.cpp huffman.h
	g++ -Wall -g -std=c++0x huffman.cpp -c

huffman_buffers.o: huffman_buffers.cpp huffman_buffers.h
	g++ -Wall -g -std=c++0x huffman_buffers.cpp -c


clean:
	rm -f *~ *.o huffman

test:
	./huffman -e < MARBLES.BMP > marbles_encode 
	./huffman -c marbles_encode < MARBLES.BMP > marbles_compressed
	./huffman -d marbles_encode < marbles_compressed > marbles_expanded
booktest:
	./huffman -e < book.txt > book.htree 
	./huffman -c book.htree < book.txt > book.huff
	./huffman -d book.htree < book.huff > book.expanded
valtest:
	valgrind ./huffman -e < MARBLES.BMP > marbles_encode 
	valgrind ./huffman -c marbles_encode < MARBLES.BMP > marbles_compressed
	valgrind ./huffman -d marbles_encode < marbles_compressed > marbles_expanded
