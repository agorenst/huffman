#include "huffman.h"

#include <list>
#include <queue>

using namespace std;
using namespace huffman;

/*
 * See huffman.h for any unclear data structures.
 * This just contains the two "main" algorithms:
 * build_tree: given a range of encodings, build a huffman tree
 * build_code: given a huffman code, build a string->string map
 * defining the encoding.
 *
 * NB: because build_tree is a template in a cpp file, we have to
 * explicitly instantiate it for each type we'd like.
 * Given the tiny size of all this program and everything, it seems unecessary,
 * but for larger programs this (and extern template) are all rather important.
 *
 * This is, of course, a C++-related note, nothing to do with the algorithm.
 *
 */

/* THE MAIN ALGORITHM */

// A lot of the "magic" happens thanks to the fact
// that the priority queue works correctly:
// hence (partly) the need for indirect_htree class.
template<class iter>
shared_ptr<htree> huffman::build_tree(iter start, iter end) {

    // this local class is critical: it implements the < operator
    // correctly.
    // Note that a priority_queue will work with shared_ptr, it just
    // won't make sense.
    struct indirect_htree : public std::shared_ptr<htree> {
        bool operator<(const indirect_htree& h) const {
            return (*this)->e < h->e;
        }
        indirect_htree(htree* h): std::shared_ptr<htree>(h) {}
    };

    // this work-queue holds our forest of trees.
    priority_queue<indirect_htree> wq;

    // we initialize the forest to n size-1 (ie, leaves)
    // trees.
    for (auto it = start; it != end; ++it) {
        wq.push(indirect_htree(new htree(*it)));
    }

    // here is the greedy algorithm: we combine the two
    // cheapest trees (be them size 1 or greater) into
    // a new tree, and put that back on the queue.
    // NB: the runtime is not as obvious from this structure as a for loop!
    // However, we see that wq.size() decreases by 1 each time.
    while (wq.size() > 1) {
        auto min1 = wq.top(); wq.pop();
        auto min2 = wq.top(); wq.pop();
        wq.push(indirect_htree(new htree(min1,min2)));
    }
    return wq.top();
}

// we use templates---extend as necessary (according to
// http://stackoverflow.com/questions/115703/storing-c-template-function-definitions-in-a-cpp-file) can't do better than this.
template
shared_ptr<htree> huffman::build_tree<list<encoding>::iterator>
(list<encoding>::iterator start, list<encoding>::iterator end);




/* THE SECOND (MUNDANE) PART OF THE ALGORITHM:
 * given a fully-built huffman tree, actually make the 
 * mapping between strings and their binary code.
 *
 * We use a queue to explore the tree---recursion would
 * be theoretically nicer, but this is C++.
 */
map<string,string> huffman::build_code(shared_ptr<htree> h) {
    typedef pair<shared_ptr<htree>, string> tree_state;

    // we ultimately map leaves of h to "binary" strings.
    map<string, string> code;

    // we're doing a BFS exploration of h.
    // at each node we save the node location (shared_ptr<htree>)
    // and the currently-built codeword (string), hence tree_state definition
    queue<tree_state> tovisit;

    // start the BFS
    tovisit.push(tree_state(h,""));
    while(tovisit.size()) {
        auto nxt = tovisit.front(); tovisit.pop();
        auto node = nxt.first;
        auto word = nxt.second;

        if (node->is_leaf()) {
            code[node->e.first] = word;
        }
        else {
            tovisit.push(tree_state(node->l,word+'0'));
            tovisit.push(tree_state(node->r,word+'1'));
        }
    }
    return code;
}
