#include <iostream>
#include <cmath>
#include <fstream>
#include <cstring>

#include "huffman.h"
//#include "feed_buffer.h"

using namespace std;

// we treat each 8-bit char as a symbol
const int numsymbs = 256;
//vector<string> encodings(numsymbs,"");
vector<double> freqs(numsymbs,0);
vector<unsigned> hist(numsymbs,0);
//
//
//
//const int buffsize = 4096;
//const unsigned char_size = 8; // we translate, eg, "01001000" into 8+64 = 72 (the character).
//
//// this takes in a file and a buffer
//// we read in a character from the infile, look up its encoding,
//// and save that encoding into the buffer.
//// (This assumes that the encoding is all character-based, etc. etc.
////
//// This returns the number of characters read from in.
//// The number of bytes written to fake_bin_buff can be computed separately
//// via the "room" method.
//unsigned feed_huff_buff(istream& in, feed_buffer<char, buffsize>& huff_buff) {
//    // we keep track of how much we've written.
//    for (unsigned i = 0; ; ++i) {
//        if (in.eof()) { return i; }
//
//        char to_encode = in.peek();
//        string encoding = encodings[to_encode];
//
//        // if we have room for this encoding (may depend on value of to_encode)
//        if (huff_buff.room() >= encoding.size()) {
//            for (auto& c : encoding) {
//                huff_buff.push(c);
//            }
//            in.get(to_encode); // pop! remove char from infile.
//        }
//        else {
//            // if huff_buff is ``full'' (or close to it), we're done.
//            return i;
//        }
//    }
//}
//
//// surely there's some fancy bit twiddling we can do,
//// but this is conceptually easier.
//char trans_bin_string(const char a[char_size]) {
//    char res = 0;
//    for (unsigned i = 0; i < char_size; ++i) {
//        if (a[i] == '1') {
//            res += static_cast<int>(pow(2,i));
//        }
//    }
//    return res;
//}
//
//// given a ``fake binary buffer'' (really, an array of characters {'0', '1'}), convert those
//// characters into real binary and save that encoding in real_bin_buff
//// This is *very* machine specific, we assume bytes = 8 bits, endian stuff (??).
//unsigned feed_bin_buff(feed_buffer<char, buffsize>& huff_buff, feed_buffer<char, buffsize>& bin_buff) {
//    char to_trans[char_size];
//    for (int i = 0; ; ++i) {
//        
//        // if we don't have enough bits to make a char, we're done
//        if (huff_buff.remaining_data() < char_size) {
//            return i;
//        }
//        // if our output buffer is full, we're done.
//        if (bin_buff.room() == 0) {
//            return i;
//        }
//        for (unsigned j = 0; j < char_size; ++j) {
//            to_trans[j] = huff_buff.pop();
//        }
//        char bin_byte = trans_bin_string(to_trans);
//        bin_buff.push(bin_byte);
//    }
//}
//
//// this simply pads the remaining data in huff_buff with 0s,
//// brining the bit count up to 8.
//// Padding with zeros is safe because of the prefix-free nature
//// ---if we simply know to stop parsing the compressed file after
//// x characters, we're good.
//void resolve_last_byte(feed_buffer<char, buffsize>& huff_buff,
//                       feed_buffer<char, buffsize>& bin_buff) {
//    if (huff_buff.remaining_data() == 0) { return; }
//    assert(huff_buff.remaining_data() < char_size);
//    assert(bin_buff.room() > 0);
//
//    // just pad with zeros and dump it to bin_buff
//    while (huff_buff.room() < char_size) {
//        huff_buff.push('0');
//    }
//    huff_to_bin(huff_buff, bin_buff);
//    assert(huff_buff.remaining_data() == 0);
//}
//
//// the hard part is dealing with the edge cases---essentially, what if 
//void write_compressed_file(istream& infile, ostream& outfile) {
//    // we declare some necessary buffers.
//    feed_buffer<char, buffsize> huff_buff, bin_buff;
//    
//
//    while (!infile.eof()) {
//        // read as many bytes as you can into fake_bin_buff
//        auto bytes_read = feed_huff_buff(infile, huff_buff);
//        // after this command, presumably huff_buff is ``full'' (or close to it).
//        
//        // take 8-byte-chunks from huff, and turn them into chars in bin_buff
//        auto bytes_tran = huff_to_bin(huff_buff, bin_buff);
//        // having emptied (or almost emptied) the huff_buff, we realign.
//        huff_buff.realign();
//
//        // the first number should be much larger than the second
//        cout << bytes_read << " " << bytes_tran << endl;
//
//        // only write to the file if the binary buffer is full
//        if (bin_buff.room() == 0) {
//            outfile.write(bin_buff.data(), bin_buff.remaining_data());
//            // because we use file writing with a buffer,
//            // we must realign ASAP!
//            bin_buff.clear();
//        }
//        // NB: there may be a really weird boundary condition
//        // when there are encodings close-to-buffsize, in
//        // which we may never progress. Characterizing this would be
//        // a useful, albeit boring, step.
//        // Presumably this will be rare, however.
//    }
//
//    // at this point we have consumed the infile.
//    // there may be a lot of data left over in huff_buff and bin_buff,
//    // so we preliminarily empty them.
//    huff_to_bin(huff_buff, bin_buff);
//    outfile.write(bin_buff.data(), bin_buff.remaining_data());
//    bin_buff.clear();
//
//    // at this point bin_buff is empty, and presumably can't be filled
//    // by whatever remains of huff_buff.
//    huff_to_bin(huff_buff, bin_buff);
//    assert(huff_buff.remaining_data() < 8);
//    assert(bin_buff.room() > 0);
//
//    // we need to "cap" the bin_buff before the final write to the file,
//    // so we've encoded the last bits.
//    resolve_last_byte(huff_buff, bin_buff);
//
//    outfile.write(bin_buff.data(), bin_buff.remaining_data());
//    // realign is needless---we should be all set.
//}
//
//
//
// read through the file.
// returns the number of bytes read.
unsigned compute_freqs(istream& in) {
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
    return total;
}
//
//unsigned load_compressed_data(istream& data) {
//    unsigned total;
//}
//
//// when we compress file.in, we generate data.out and comp.out.
//// data.out contains plaintext version of our encoding data.
//// Obviously this should all be in one compressed file, but, baby steps.
//void save_encoding_data(ostream& datafile, unsigned size) {
//    datafile << size << endl;
//    for (const auto& enc : encodings) {
//        datafile << enc << endl;
//    }
//}

enum MODE { FREQ, COMP, DECOMP };
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
        auto tree = read_htree(tree_file);
        cerr << "Done reading in tree" << endl;
        write_htree(tree, cout);
    }
    else {
        cout << usage_string;
        return 0;
    }
}
