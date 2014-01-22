#include <iostream>
#include <cmath>
#include <fstream>
#include <cstring>
#include <array>
#include <cassert>

#include "huffman.h"
#include "huffman_buffers.h"

using namespace std;

// Our core system assumptions are that an unsigned
// character is an 8-bit pattern, where
// unsigned char b = 0 -> b == 00000000.
// As our "alphabet" for huffman encoding is also
// the 256 possible 8-bit patters, we abuse notation
// and let the index
typedef unsigned char byte;
// we treat each 8-bit char as a symbol
// this can change, thanks to our indexing model.
const int numsymbs = 256;


// This is a global variable because everything uses
// exactly one huffman tree, so why pass it all over
// the place.
shared_ptr<htree> hufftree;


void encode(istream& infile, ostream& outfile) {
    encode_buffer huffbuff(numsymbs, hufftree);

    while(!infile.eof()) {
        bool buffer_has_room = true;
        while(buffer_has_room) {
            byte c = infile.peek();
            
            buffer_has_room = huffbuff.put_byte(c);

            // if we successfully copied, pop
            if (buffer_has_room) {
                c = infile.get();
            }
        }

        bool buffer_has_results = true;
        while(buffer_has_results) {
            byte c;

            buffer_has_results = huffbuff.pop_byte(c);
            
            if (buffer_has_results) {
                outfile.put(c);
            }
        }
    }

    assert(infile.eof());
    //assert(huffbuff.next_empty < byte_size);

    byte c;

    bool buffer_terminate = huffbuff.terminate(c);
    assert(buffer_terminate);
    outfile.put(c);
}


void decode(istream& infile, ostream& outfile, const int bytes_to_write=4264316) {
    int written = 0;
    decode_buffer huffbuff(numsymbs, hufftree);

    while(!infile.eof()) {
        bool buffer_has_room = true;
        while(buffer_has_room) {
            byte c = infile.peek();
            
            buffer_has_room = huffbuff.put_byte(c);

            // if we successfully copied, pop
            if (buffer_has_room) {
                c = infile.get();
            }
        }

        bool buffer_has_results = true;
        while(buffer_has_results) {
            byte c;

            buffer_has_results = huffbuff.pop_byte(c);
            
            if (buffer_has_results) {
                outfile.put(c);
                written++;
            }
            if (written == bytes_to_write) {
                return;
            }
        }
    }
}

// read through the file, compute frequency table.
// returns the number of bytes read.
unsigned compute_freqs(istream& in, vector<double>& freqs) {
    vector<unsigned> hist(numsymbs,0);
    unsigned total = 0;
    while (!in.eof()) {
        byte a = in.get();
        hist[a] += 1;
        total += 1;
    }
    for (int i = 0; i < freqs.size(); ++i) {
        freqs[i] = static_cast<double>(hist[i])/
                   static_cast<double>(total);
    }
    return total;
}




const string usage_string = "Usage: ./huffman -e < file > encoding_description\n"
                            "       ./huffman -c encoding_description < orig_file > compression\n"
                            "       ./huffman -d encoding_description < compression > orig_file\n";

int main(int argc, char* argv[]) {
    vector<string> encodings(numsymbs,"");
    if (argc == 2 && strncmp(argv[1],"-e",2) == 0) {
        vector<double> freqs(numsymbs,0);
        auto bytes_read = compute_freqs(cin, freqs);
        auto tree = generate_tree(freqs);
        cout << bytes_read - 1 << " ";
        write_htree(tree, cout);
        cout << endl;
    }
    else if (argc == 3 && strncmp(argv[1], "-p",2) == 0) {
        ifstream tree_file(argv[2]);
        unsigned bytecount;
        tree_file >> bytecount;
        hufftree = read_htree(tree_file);
        generate_encodings(hufftree, encodings);
        for (int b = 0; b < encodings.size(); ++b) {
            // if symbol with index b is not in the file,
            // we may choose to not include it in our huffman
            // tree
            if (encodings[b] != "") {
                cout << b << " " << encodings[b] << endl;
            }
        }
    }
    else if (argc == 3 && strncmp(argv[1], "-c",2) == 0) {
        ifstream tree_file(argv[2]);
        unsigned bytecount;
        tree_file >> bytecount;
        hufftree = read_htree(tree_file);
        encode(cin, cout);
    }
    else if (argc == 3 && strncmp(argv[1], "-d",2) == 0) {
        ifstream tree_file(argv[2]);
        unsigned bytecount;
        tree_file >> bytecount;
        hufftree = read_htree(tree_file);
        decode(cin, cout, bytecount);
    }
    else {
        cout << usage_string;
        return 0;
    }
}
