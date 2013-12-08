#include "huffman.h"
#include <string>
#include <map>

#include <list>
#include <memory> // shared_ptr
#include <queue> // priority_queue
#include <iostream>
#include <cmath>

using namespace std;


/*
 * STARTING PRINTING FUNCTIONS
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
/*
 * ENDING PRINTING FUNCTIONS
 */




int main(int argc, char* argv[]) {
    string s;
    double f;
    list<huffman::encoding> encodings;
    while(!cin.eof()) {
        cin >> s;
        cin >> f;
        encodings.push_back(huffman::encoding(s,f));
    }
    // weird error with input reading loop, so we loop once more.
    // easy remedy. TODO how set eof before failure? I see what's going on...
    encodings.pop_back();

    // from the basic encodings, build the huffman tree.
    auto h = huffman::build_tree(encodings.begin(), encodings.end());

    //cout << h << endl;
    // recursively traverse the huffman tree to build the encoding.
    //auto m = start_coding_map(h);
    auto m = huffman::build_code(h);
//
//    // sending encoding to stdout.
    for (auto it = m.begin(); it != m.end(); ++it) {
        cout << it->first << " " << it->second << endl;
    }
    //cout << "Cost: " << compute_cost(encodings.begin(), encodings.end(), m) << endl;
    //cout << "Entropy: " << compute_entropy(encodings.begin(), encodings.end()) << endl;
}
