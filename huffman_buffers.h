#include "huffman.h"

#include <array>
#include <vector>
#include <string>
#include <memory>

const int buffsize = 4096;
typedef unsigned char byte;
const unsigned byte_size = 8; // we translate, eg, "01001000" into 8+64 = 72 (the character).

class encode_buffer {
    public:
        // our actual buffer
        std::array<bool,buffsize> m;
        

        std::vector<std::string> encodings;
        unsigned next_empty;// = 0;
    public:


        // the constructor essentially initializes its huffman encoding table.
        encode_buffer(const unsigned numsymb, const std::shared_ptr<htree> h):
            encodings(numsymb,""), next_empty(0) {
            generate_encodings(h, encodings);
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
        const std::shared_ptr<htree> hufftree;
        void slide_array();
    public:
        decode_buffer(const unsigned numsymb, const std::shared_ptr<htree> h):
            next_empty(0), first_data(0), hufftree(h) {}

        bool put_byte(const byte b);
        bool pop_byte(int& b);
};

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
