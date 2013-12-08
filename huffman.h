#ifndef _HUFFMAN_H_
#define _HUFFMAN_H_

#include <map>
#include <string>
#include <memory>

/*
 * This header file defines the huffman namespace.
 * It has 2 data structures:
 *    encoding is a pair<string, double> with a < and + operator.
 *    htree is the basic tree, with data=encoding, and limited constructors.
 * What is left declared, but undefined, is build_tree, which is the
 * "main" huffman encoding greedy algorithm, and "build_code",
 * which is just a BFS exploration of the tree, building the binary string.
 *
 * Those are defined in huffman.cpp
 *
 */

namespace huffman {
    /* KEY DATA STRUCTURES */

    // The huffman algorithm takes as input a sequence of <symbol, frequency> pairs.
    // These are stored in this structure, which is basically a tuple with < and + operators.
    class encoding : public std::pair<std::string, double> {
        public:
            encoding(std::string s, double f): std::pair<std::string, double> (s, f) {}

            // we create an ordering, core is based on frequency (second)
            bool operator<(const encoding& e) const {
                if (second == e.second) { return first < e.first; }
                return second > e.second;
            }
            encoding operator+(const encoding& e) const {
                return encoding(first+e.first, second+e.second);
            }
    };

    // the huffman algorithm iteratively builds up a binary tree.
    // This is the basic recursive tree we use.
    // Observe that all fields are const, and we only have 2 constructors
    // (one for 0-children, one for 2-children).
    struct htree {
        const std::shared_ptr<htree> l, r;
        const encoding e;
        htree(std::shared_ptr<htree> l, std::shared_ptr<htree> r): l(l), r(r), e(l->e + r->e) {}
        htree(encoding e): l(), r(), e(e) {}
        bool is_leaf() const { return l == NULL && r == NULL; }
    };



    template<class iter>
        std::shared_ptr<htree> build_tree(iter start, iter end);

    std::map<std::string,std::string> build_code(std::shared_ptr<htree> h);

};

#endif
