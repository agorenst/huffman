#include <iostream>
#include <cmath>
#include <fstream>
#include <cstring>
#include <array>
#include <cassert>

#include "huffman.h"
#include "huffman_buffers.h"

using namespace std;
using namespace huffman;
using namespace huffman_buffers;

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


// One of the two "main" functions, this encodes what it sees
// out of infile and writes the encoding to outfile.
// The actual byte-to-huffman logic takes place in the 
// encode_buffer huffbuff---here we just put and pop.
void encode(istream& infile, ostream& outfile) {
    encode_buffer huffbuff(numsymbs, hufftree);

    while(!infile.eof()) {


        // we fill up the inbuffer as best we can
        bool buffer_has_room = true;
        while(buffer_has_room) {
            byte c = infile.peek();
            
            buffer_has_room = huffbuff.put_byte(c);

            // if we successfully copied, pop
            if (buffer_has_room) {
                c = infile.get();
            }
        }


        // then we write as much as we can
        bool buffer_has_results = true;
        while(buffer_has_results) {
            byte c;

            buffer_has_results = huffbuff.pop_byte(c);
            
            if (buffer_has_results) {
                outfile.put(c);
            }
        }

    }

    // at this point there should just be a terminal byte-fragment left
    // in the buffer, and nothing in the file.
    assert(infile.eof());
    assert(huffbuff.next_empty - huffbuff.first_data < byte_size);

    // the final byte.
    byte c;

    bool buffer_terminate = huffbuff.terminate(c);
    assert(buffer_terminate);
    outfile.put(c);
}


// This logic is much like encoding.
// To deal with the perhaps-troublesome "terminal" byte, we remember
// how many bytes were in the original file (bytes_to_write).
// Otherwise, this one is a bit simpler.
void decode(istream& infile, ostream& outfile, const int bytes_to_write) {
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
            // technically this is an index, but we use it as a char
            int index;

            buffer_has_results = huffbuff.pop_byte(index);
            byte c(index); 

            
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

unsigned compute_counts(istream& in, vector<unsigned>& counts) {
    unsigned total = 0;
    while (!in.eof()) {
        byte a = in.get();
        counts[a] += 1;
        total += 1;
    }
    return total;
}

// read through the file, compute frequency table.
// returns the number of bytes read.
unsigned compute_freqs(istream& in, vector<double>& freqs) {
    vector<unsigned> hist(numsymbs,0);
    unsigned total = compute_counts(in, hist);
    for (unsigned i = 0; i < freqs.size(); ++i) {
        freqs[i] = static_cast<double>(hist[i])/
                   static_cast<double>(total);
    }
    return total;
}


/* These two functions are largely for verifying that we have a good
 * encoding */

double compute_entropy(const vector<double>& freqs) {
    double cost = 0;
    for (auto& d : freqs) {
        if (d > 0) {
            cost += -(log(d)/log(2))*d;
        }
    }
    return cost;
}

double compute_cost(const vector<double>& freqs, const vector<string>& encodings) {
    double cost = 0;
    for (unsigned i = 0; i < freqs.size(); ++i) {
        cost += encodings[i].size()*freqs[i];
    }
    return cost;
}


const string usage_string = "Usage: ./huffman -e < file > encoding_description\n"
                            "       ./huffman -c encoding_description < orig_file > compression\n"
                            "       ./huffman -d encoding_description < compression > orig_file\n";

int main(int argc, char* argv[]) {
    vector<string> encodings(numsymbs,"");

    // mode: scan through CIN and create an "encoding file"
    // which is the file-size in bytes, and a linear htree
    // description. This helper file is used to decode
    // and encode the original file
    if (argc == 2 && strncmp(argv[1],"-e",2) == 0) {
        vector<double> freqs(numsymbs,0);
        auto bytes_read = compute_freqs(cin, freqs);
        hufftree = generate_tree(freqs);
        cout << bytes_read - 1 << " ";
        write_htree(hufftree, cout);
        cout << endl;
        generate_encodings(hufftree, encodings);
        cerr << "Entropy: " << compute_entropy(freqs) << "\tCost: " << compute_cost(freqs,encodings) << endl;
    }

    // mode: print out the encodings implied by a huffman
    // tree file passed in via CIN
    else if (argc == 2 && strncmp(argv[1], "-p",2) == 0) {
        unsigned bytecount;
        cin >> bytecount;
        hufftree = read_htree(cin);
        generate_encodings(hufftree, encodings);
        for (unsigned b = 0; b < encodings.size(); ++b) {
            // if symbol with index b is not in the file,
            // we may choose to not include it in our huffman
            // tree
            if (encodings[b] != "") {
                cout << b << " " << encodings[b] << endl;
            }
        }
    }

    // mode: print the relative frequencies of each byte in the file.
    // May help indicate how well a compression may go, or will go.
    else if (argc == 2 && strncmp(argv[1], "-f",2) == 0) {
        vector<double> freqs(numsymbs,0);
        compute_freqs(cin, freqs);
        for (unsigned i = 0; i < freqs.size(); ++i) {
            cout << freqs[i] << endl;
        }
    }

    // mode: given a file describing a hufftree (-e mode)
    // create the huffman compression of CIN.
    else if (argc == 3 && strncmp(argv[1], "-c",2) == 0) {
        ifstream tree_file(argv[2]);
        // something went wrong opening the file
        if (!tree_file.is_open()) {
            cerr << "Error, file " << *(argv[2]) << " could not be opened\n"
                    "to define the htree." << endl;
        }
        unsigned bytecount;
        tree_file >> bytecount;
        hufftree = read_htree(tree_file);
        encode(cin, cout);
    }

    // mode: given a file decsribing a hufftree (created by -e)
    // decode the huffman compression of that original file.
    else if (argc == 3 && strncmp(argv[1], "-d",2) == 0) {
        ifstream tree_file(argv[2]);
        if (!tree_file.is_open()) {
            cerr << "Error, file " << *(argv[2]) << " could not be opened\n"
                    "to define the htree." << endl;
        }
        unsigned bytecount;
        tree_file >> bytecount;
        hufftree = read_htree(tree_file);
        decode(cin, cout, bytecount);
    }
    
    // oopsies
    else {
        cout << usage_string;
        return 0;
    }
}
