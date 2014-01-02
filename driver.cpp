#include <iostream>
#include <array>
#include <map>
#include <cmath>
#include <fstream>

#include "huffman.h"

using namespace std;

const int numsymbs = 256;
vector<string> encodings(numsymbs,"");
vector<double> freqs(numsymbs,0);
vector<unsigned> hist(numsymbs,0);


/* to facilitate encoding and decoding, we have to:
 *   load data into a buffer
 *   transform that data into intermediate form (fake binary)
 *   transform that intermediate form into real binary
 */

// maybe rename to "feed buffer".
// Really two stages, if one wants to be consistent
template<class T, unsigned S>
class feed_buffer : public array<T, S> {
    private:
    unsigned next_empty = 0;
    unsigned consumed = 0;
    public:

    // invariant: S >= next_empty
    unsigned room() const { return S - next_empty; }
    unsigned toread() const { return next_empty - consumed; }

    // invariant (assumption must be maintained by user: room() >= l
    void append(const T* a, unsigned l) { 
        for (unsigned i = 0; i < l; ++i) {
            push(a[i]);
        }
    }

    // invariant: next_empty - consumed > 0;
    T& pop() {
        ++consumed;
        return (*this)[consumed-1];
    }

    void push(const T& d) {
        (*this)[next_empty++] = d;
    }

    void reset() {
        copy(array<T,S>::begin()+consumed, array<T,S>::begin()+next_empty, array<T,S>::begin());
        next_empty -= consumed;
        consumed = 0;
    }
};

const int buffsize = 4096;
const unsigned char_size = 8; // we translate, eg, "01001000" into 8+64 = 72 (the character).

// this takes in a file, a buffer, and a huffman encoding map.
// we read in a character from the infile, look up its encoding,
// and save that encoding into the buffer.
// (This assumes that the encoding is all character-based, etc. etc.
//
// This returns the number of characters read from in.
// The number of bytes written to fake_bin_buff can be computed separately
// via the "room" method.
unsigned file_to_huff(istream& in, feed_buffer<char, buffsize>& fake_bin_buff) {
    for (int i = 0; ; ++i) {
        char to_encode = in.peek();

        // hooray, we're done!
        if (in.eof()) { return i; }

        // otherwise, try to save the encoding of that character into the buffer
        string encoding = encodings[to_encode];
        if (fake_bin_buff.room() >= encoding.size()) {
            fake_bin_buff.append(encoding.c_str(), encoding.size());
            in.get(to_encode); // really we're 'popping' here.
        }
        // if the buffer doesn't have room, we're done.
        else {
            return i;
        }
    }
}



// surely there's some fancy bit twiddling we can do,
// but this is conceptually easier.
char trans_bin_string(const char a[char_size]) {
    char res = 0;
    for (unsigned i = 0; i < char_size; ++i) {
        if (a[i] == '1') {
            res += static_cast<int>(pow(2,i));
        }
    }
    return res;
}

// given a ``fake binary buffer'' (really, an array of characters {'0', '1'}), convert those
// characters into real binary and save that encoding in real_bin_buff
// This is *very* machine specific, we assume bytes = 8 bits, endian stuff (??).
unsigned huff_to_bin(feed_buffer<char, buffsize>& fake_bin_buff, feed_buffer<char, buffsize>& real_bin_buff) {
    char to_trans[char_size]; // this holds the next 8 chars in fake_bin_buff to be compressed to a single byte.
    for (int i = 0; ; ++i) {

        // if there's not enough bytes to translate
        if (fake_bin_buff.toread() < char_size) {
            return i; 
        }
        
        // if there's no room to store a new char
        if (real_bin_buff.room() == 0) {
            return i;
        }

        // this is the actual put-into-buff part.
        for (unsigned j = 0; j < char_size; ++j) {
            to_trans[j] = fake_bin_buff.pop();
        }
        char bin_byte = trans_bin_string(to_trans);
        real_bin_buff.push(bin_byte);
    }
}

// the hard part is dealing with the edge cases---essentially, what if 
void write_compressed_file(istream& infile, ostream& outfile) {
    // we declare some necessary buffers.
    feed_buffer<char, buffsize> fake_bin_buff;
    feed_buffer<char, buffsize> real_bin_buff;
    
    // read as many bytes as you can into fake_bin_buff
    auto bytes_read = file_to_huff(infile, fake_bin_buff);

    // take as many 8-byte-chunks from fake, and turn them into chars in real_bin.
    auto bytes_tran = huff_to_bin(fake_bin_buff, real_bin_buff);

    cout << bytes_read << " " << bytes_tran << endl;
    // clears out the transferred memory.
    // there must be a better way of thinking about buffers...
    fake_bin_buff.reset();

    // only write to the file if the binary buffer is full
    if (real_bin_buff.room() == 0) {
        outfile.write(real_bin_buff.data(), buffsize);
    }

}



// read through the file.
void compute_freqs(istream& in) {
    unsigned total = 0;
    while (!in.eof()) {
        unsigned char a = in.get();
        hist[a] += 1;
        total += 1;
    }
    for (int i = 0; i < numsymbs; ++i) {
        freqs[i] = static_cast<double>(hist[i])/
                   static_cast<double>(total);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: ./huffman <input file> <output file>" << endl;
        return 0;
    }

    // this block computes frequency.
    // why a block? mainly so freq_computer is destroyed,
    // closing the file nicely. Probably restructured soon.
    {
        ifstream freq_computer;
        freq_computer.open(argv[1]);
        compute_freqs(freq_computer);
    }

    // freqs are in the inputs, and encodings the "output"
    huffman_encode(freqs, encodings);

    ifstream input_to_compress;
    // we open the input file twice---dumb!
    input_to_compress.open(argv[1]);
    ofstream compressed;
    compressed.open(argv[2], ofstream::binary);

    write_compressed_file(input_to_compress, compressed);

}
