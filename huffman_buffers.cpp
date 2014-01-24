#include "huffman_buffers.h"

#include <cassert>

using namespace std;

using namespace huffman_buffers;


// HELPER FUNCTIONS FOR ENCODE BUFFER METHODS

// these are helper values for the encode buffer implementation, namely
// the "pop_byte" subroutine (see the usage of "bits_to_byte").

class bit_to_byte_masks : public array<byte, 8> {
    public:
    bit_to_byte_masks() {
        for (auto i = 0; i < 8; ++i) {
            (*this)[i] = 1 << i;
        }
    }
};

// this value is to help with the encoding process
bit_to_byte_masks masks;

byte bits_to_byte(const array<bool,buffsize>& b, const unsigned start, const unsigned n = 8) {
    // i hope this is an actual all-zero pattern.
    byte d = 0;
    for (unsigned i = start; i < start+n; ++i) {
        if (b[i]) {
            d |= masks[i-start];
        }
    }
    return d;
}

// because files take 8-bit chunks while huffman encoding can have many-lengths,
// sometimes we get an array with something like 5 bits left over, with room for 2 more bits.
// A naive implementation could just slide the array as soon as possible (essentially for every
// byte popped), but here we use first_data to avoid this big-copies too much.
void slide_array(std::array<bool,buffsize>& m, unsigned& first_data, unsigned& next_empty) {
    assert(first_data <= next_empty);
    //cerr << "Sliding buffer with first data: " << first_data << " and next_empty: " << next_empty << endl;
    copy(m.begin()+first_data, m.begin()+next_empty, m.begin());
    // subtract first_data from both indices.
    next_empty -= first_data;
    first_data = 0;
}

// ENCODE BUFFER METHODS

bool encode_buffer::put_byte(const int b) {

    if (encodings[b].size() > buffsize - next_empty) {
        slide_array(m, first_data, next_empty);
    }
    // this dumb arithmetic can play havoc with off-by-one errors. Careful!
    if (encodings[b].size() <= (buffsize - next_empty)) {
        for (auto& c : encodings[b]) {
            m[next_empty++] = c == '1';
        }
        return true;
    }
    return false;
}

bool encode_buffer::pop_byte(byte& b) {
    // eg, if next_empty is 8, then [0..7] is defined, and that's a bytee
    // this dumb arithmetic can play havoc with off-by-one errors. Careful!
    if (byte_size <= next_empty - first_data) {
        b = bits_to_byte(m, first_data, byte_size);
        // having now popped the first 8 bits into b, we must
        // slide our memory back (not efficient! i know...)
        //copy(m.begin()+byte_size, m.begin()+next_empty, m.begin());
        first_data += byte_size;
        return true;
    }
    return false;
}


bool encode_buffer::terminate(byte& b) {
    slide_array(m, first_data, next_empty);
    // we have 7 or fewer bits...
    // this dumb arithmetic can play havoc with off-by-one errors. Careful!
    if (next_empty < byte_size) {
        b = bits_to_byte(m, next_empty, byte_size);
        next_empty = 0;
        return true;
    }
    return false;
}

// DECODE BUFFER METHODS

bool decode_buffer::put_byte(const byte b) {

    // the first_data comparison value (buffsize/2) is fairly arbitrary.
    // it's just a stop-gap to slide the array all the time...
    if (byte_size > buffsize - next_empty) {
        slide_array(m, first_data, next_empty);
    }

    // if buffsize is 256 (say), then
    // the last index is 255, and say we have next_empty = 248,
    // then 248, 249, 250, 251, 252, 253, 254, 255 are empty.
    // and 256-248 = 8
    // this dumb arithmetic can play havoc with off-by-one errors. Careful!
    if (buffsize - next_empty >= byte_size) {
        for (unsigned i = 0; i < byte_size; ++i) {
            // i think this works...?
            m[next_empty++] = (b & masks[i]) == masks[i];
        }
        return true;
    }
    return false;
}

bool decode_buffer::pop_byte(int& b) {
    // we just traverse the tree, so we don't need
    // to waste time with shared_ptr increment stuff
    auto t = hufftree.get();
    unsigned bit_loc = first_data;

    // while we may still have a full tree-path in the buffer...
    while(bit_loc < next_empty) {
        if (t->is_leaf()) {
            b = t->i;

            // we've popped out this data, so we increment first_data
            //copy(m.begin()+bit_loc, m.begin()+next_empty, m.begin());
            //next_empty -= bit_loc;
            first_data = bit_loc;
            //cerr << "Pop byte updates first data: " << first_data << endl;
            return true;
        }
        if (m[bit_loc++]) {
            t = t->r.get();
        }
        else {
            t = t->l.get();
        }
    }
    return false;
}
