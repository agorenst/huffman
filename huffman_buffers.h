#ifndef _HUFFMAN_BUFFERS_H_
#define _HUFFMAN_BUFFERS_H_

#include "huffman.h"

#include <array>
#include <vector>
#include <string>
#include <memory>


/*
* This file defines a buffer system which handles, essentially, IO to
* huffman-encoded files. If you want to actually compress files using
* the huffman encoding module, you need this module.
*
* The two classes are encode_buffer and decode_buffer, their main methods
* both being called "put_byte" and "pop_byte".
* 
* Conceptually, you pipe in the original file through the encode_buffer.
* It modifies that stream into its huffman-encoded form, which comes out
* the other end.
* So you feed in your input.txt byte-by-byte via "put_byte", and extract
* out the compressed file byte-by-byte via "pop_byte".
*
* decode_buffer has the same idea: you put in the compressed file byte-by-byte,
* and extract out the decoded file byte-by-byte.
*
* The main tricky part is that, say, when encoding 'a' from the input file,
* it may be huffman-encoded as the 3 bits 001. The problem is we can only
* write bytes to a file. Thus, in encode_buffer, we'd want to dump in lots of
* input characters, then extract out what bytes we can. Hence, buffering is nice
* and, in a way, necessary.
* NB "terminate" deals with the case when the huffman encoded file does not
* nicely give us a multiple-of-8 bits in the result.
* 
* There's a similar, but easier, manifestation of this issue in decode_buffer.
*
* Last issue: the buffer is set to 4096 bytes (well, bools). In a degenerate case
* (when a symbol's huffman-encoding exceeds this-minus-a-byte), it would essentially
* over-fill the buffer and things will fail. The user of this module should check
* that their huffman encoding isn't very large.
*/

namespace huffman_buffers {

    // a general rule-of-thumb: buffers a nice memory size. May not matter here.
    // The buffer *must* have enough room to store at least the longest single code work,
    // plus a byte-size.
    const int buffsize = 4096*8;
    typedef unsigned char byte;
    const unsigned byte_size = 8; // we translate, eg, "01001000" into 8+64 = 72 (the character).

    class encode_buffer {
        public:
            // our actual buffer
            std::array<bool,buffsize> m;

            std::vector<std::string> encodings;
            unsigned next_empty;// = 0;
            unsigned first_data;// = 0
        public:


            // the constructor essentially initializes its huffman encoding table.
            encode_buffer(const unsigned numsymb, const std::shared_ptr<huffman::htree> h):
                encodings(numsymb,""), next_empty(0), first_data(0) {
                    huffman::generate_encodings(h, encodings);
                }

            // we use ints as an ``index'' into the encoding.
            // That's the model we use, --- as it happens in our
            // full implementation, the indices *are* the unsigned char valuse
            // of what we want to write.
            bool put_byte(const int b);

            // this tries to give us a byte (unsigned char), and the return value
            // indicates if it thinks it has succeeded or not.
            bool pop_byte(byte& b);

            // only use when contents < byte_size!
            bool terminate(byte& b);
    };

    class decode_buffer {
        public:
            std::array<bool,buffsize> m;
            // can't do the c++11 thing yet, for g++ compatibility
            unsigned next_empty;// = 0;
            unsigned first_data;// = 0; 
            const std::shared_ptr<huffman::htree> hufftree;
        public:
            decode_buffer(const unsigned numsymb, const std::shared_ptr<huffman::htree> h):
                next_empty(0), first_data(0), hufftree(h) {
                }

            bool put_byte(const byte b);
            bool pop_byte(int& b);
    };
}

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

#endif
