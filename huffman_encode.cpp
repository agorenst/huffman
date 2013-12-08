#include "huffman.h"
#include <string>
#include <map>

#include <list>
#include <memory> // shared_ptr
#include <iostream>

using namespace std;

/*
 * This is a simple "driver" program.
 * Given a frequency file (the format is rather obvious)
 * on stdin, we output the huffman encoding to stdout.
 * (Note: the "binary strings" are actually normal character
 * strings, but outputting binary to stdout would be... interesting.)
 *
 */


ostream& operator<<(ostream& o, const huffman::encoding& e) {
    o << e.first << ": " << e.second;
    return o;
}

// here is the infix printing of the htree itself.
ostream& operator<<(ostream& o, shared_ptr<huffman::htree> h) {
    if (h == NULL) { return o; }
    o << h->e << endl << h->l << h->r;
    return o;
}


int main(int argc, char* argv[]) {
    string s;
    double f;
    list<huffman::encoding> encodings;
    for (;;) {
        cin >> s;
        cin >> f;
        // if the eof flag is set, that means s, f haven't changed.
        if (cin.eof()) { break; }
        encodings.push_back(huffman::encoding(s,f));
    }

    // from the basic encodings, build the huffman tree.
    auto h = huffman::build_tree(encodings.begin(), encodings.end());

    // recursively traverse the huffman tree to build the encoding.
    auto m = huffman::build_code(h);

    // sending encoding to stdout.
    for (auto it = m.begin(); it != m.end(); ++it) {
        cout << it->first << " " << it->second << endl;
    }
}
