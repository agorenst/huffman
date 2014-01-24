#ifndef _NEW_HUFFMAN_H_
#define _NEW_HUFFMAN_H_

#include <vector>
#include <memory>
#include <iostream>
#include <string>

// this defines the core structures and functions
// for creating, printing, and otherwise looking at
// huffman trees.
namespace huffman {

    /* the htree object is a tree which stores in its leaves indices
     * to the symbols */
    struct htree {

        // not constant because of read_htree;
        std::shared_ptr<htree> l, r;
        const double w;
        int i;


        // leaf: it stores an index to the symbol (i), and the freq (w)
        htree(double w, unsigned i): w(w), i(i) {}

        // node: it has weight equal to its children, and no symbol (i)
        htree(std::shared_ptr<htree> l, std::shared_ptr<htree> r):
            l(l), r(r), w(l->w + r->w), i(-1) {}

        // we check both children because read_htree builds things iteratively.
        bool is_leaf() const { return l == NULL && r == NULL; }

        // defined because of read_htree
        htree(): l(NULL), r(NULL), w(0), i(-1) {}

    };

    std::shared_ptr<htree> generate_tree(const std::vector<double>& freqs);

    void write_htree(const std::shared_ptr<htree> h, std::ostream& o);

    // this builds an htree, but doesn't fill in the weights (w).
    // so be wary?
    std::shared_ptr<htree> read_htree(std::istream& in);

    void generate_encodings(const std::shared_ptr<htree> h, std::vector<std::string>& encodings);

}

#endif
