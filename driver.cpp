#include <iostream>
#include <cmath>
#include <fstream>
#include <cstring>
#include <array>
#include <cassert>

#include "huffman.h"
//#include "feed_buffer.h"

using namespace std;

// we treat each 8-bit char as a symbol
const int numsymbs = 256;
vector<string> encodings(numsymbs,"");
shared_ptr<htree> hufftree;
vector<double> freqs(numsymbs,0);
vector<unsigned> hist(numsymbs,0);
const int buffsize = 4096;
const unsigned byte_size = 8; // we translate, eg, "01001000" into 8+64 = 72 (the character).

typedef unsigned char byte;

class bit_to_byte_masks : public array<byte, 8> {
    public:
    bit_to_byte_masks() {
        for (auto i = 0; i < 8; ++i) {
            (*this)[i] = 1 << i;
        }
    }
};

bit_to_byte_masks masks;

byte bits_to_byte(const array<bool,buffsize>& b, const unsigned n = 8) {
    // i hope this is an actual all-zero pattern.
    byte d = 0;
    for (int i = 0; i < n; ++i) {
        if (b[i]) {
            d |= masks[i];
        }
    }
    return d;
}

class encode_buffer {
    public:
        // default constructor...?
        array<bool,buffsize> m;
        unsigned next_empty = 0;
    public:
        bool put_byte(const byte b) {
            if (encodings[b].size() < (buffsize - next_empty)) {
                for (auto& c : encodings[b]) {
                    m[next_empty++] = c == '1';
                }
                return true;
            }
            return false;
        }
        bool pop_byte(byte& b) {
            // eg, if next_empty is 8, then [0..7] is defined, and that's a bytee
            if (byte_size <= next_empty) {
                b = bits_to_byte(m);
                // having now popped the first 8 bits into b, we must
                // slide our memory back (not efficient! i know...)
                copy(m.begin()+byte_size, m.begin()+next_empty, m.begin());
                next_empty -= 8;
                return true;
            }
            return false;
        }

        // only use when contents < 8!
        bool terminate(byte& b) {
            // we have 7 or fewer bits...
            if (next_empty < byte_size) {
                b = bits_to_byte(m, next_empty);
                next_empty = 0;
                return true;
            }
            return false;
        }
};

void encode(istream& infile, ostream& outfile) {
    encode_buffer huffbuff;

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
    assert(huffbuff.next_empty < byte_size);

    byte c;

    bool buffer_terminate = huffbuff.terminate(c);
    assert(buffer_terminate);
    outfile.put(c);
}

class decode_buffer {
    public:
        // default constructor...?
        array<bool,buffsize> m;
        unsigned next_empty = 0;
    public:
        bool put_byte(const byte b) {
            // if buffsize is 256 (say), then
            // the last index is 255, and say we have next_empty = 248,
            // then 248, 249, 250, 251, 252, 253, 254, 255 are empty.
            // and 256-248 = 8
            if (buffsize - next_empty >= byte_size) {
                for (auto i = 0; i < byte_size; ++i) {
                    // i think this works...?
                    m[next_empty++] = (b & masks[i]) == masks[i];
                }
                return true;
            }
            return false;
        }
        bool pop_byte(byte& b) {
            auto t = hufftree;
            unsigned bit_loc = 0;

            // while we may still have a full tree-path in the buffer...
            while(bit_loc < next_empty) {
                if (t->is_leaf()) {
                    b = t->i;
                    copy(m.begin()+bit_loc, m.begin()+next_empty, m.begin());
                    next_empty -= bit_loc;
                    return true;
                }
                if (m[bit_loc++]) {
                    t = t->r;
                }
                else {
                    t = t->l;
                }
            }
            return false;
        }
};

void decode(istream& infile, ostream& outfile) {
    const int bytes_to_write = 16;
    int written = 0;
    decode_buffer huffbuff;

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
//
// read through the file.
// returns the number of bytes read.
unsigned compute_freqs(istream& in) {
    unsigned total = 0;
    while (!in.eof()) {
        byte a = in.get();
        hist[a] += 1;
        total += 1;
    }
    for (int i = 0; i < numsymbs; ++i) {
        freqs[i] = static_cast<double>(hist[i])/
                   static_cast<double>(total);
    }
    return total;
}



// this "works", from my ad-hoc testing. That outputs 17, for instansce.
//    array<bool,buffsize> a;
//    a[0] = 1;
//    a[1] = 0;
//    a[2] = 0;
//    a[3] = 0;
//    a[4] = 1;
//    a[5] = 0;
//    a[6] = 0;
//    a[7] = 0;
//    cout << (int)bits_to_byte(a) << endl;

const string usage_string = "Usage: ./huffman -e < file > encoding_description\n"
                            "       ./huffman -c encoding_description < orig_file > compression\n"
                            "       ./huffman -d encoding_description < compression > orig_file\n";

int main(int argc, char* argv[]) {
    if (argc == 2 && strncmp(argv[1],"-e",2) == 0) {
        cerr << "Encoding!" << endl;
        auto bytes_read = compute_freqs(cin);
        auto tree = generate_tree(freqs);
        cerr << "Bytes read: " << bytes_read << endl;
        write_htree(tree, cout);
        cout << endl;
    }
    else if (argc == 3 && strncmp(argv[1], "-c",2) == 0) {
        cerr << "Compressing!" << endl;
        ifstream tree_file(argv[2]);
        hufftree = read_htree(tree_file);
        generate_encodings(hufftree, encodings);
        encode(cin, cout);
    }
    else if (argc == 3 && strncmp(argv[1], "-d",2) == 0) {
        cerr << "Decompressing!" << endl;
        ifstream tree_file(argv[2]);
        hufftree = read_htree(tree_file);
        decode(cin, cout);
    }
    else {
        cout << usage_string;
        return 0;
    }
}
