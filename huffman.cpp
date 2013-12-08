#include "huffman.h"

#include <list>

using namespace std;
using namespace huffman;

/* THE MAIN ALGORITHM */

// A lot of the "magic" happens thanks to the fact
// that the priority queue works correctly:
// hence (partly) the need for indirect_htree class.
template<class iter>
shared_ptr<htree> huffman::build_tree(iter start, iter end) {

    // this work-queue holds our forest of trees.
    std::priority_queue<indirect_htree> wq;

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

// we use templates---extend as necessary (according to http://stackoverflow.com/questions/115703/storing-c-template-function-definitions-in-a-cpp-file) can't do better than this.
template
shared_ptr<htree> huffman::build_tree<list<encoding>::iterator>(list<encoding>::iterator start, list<encoding>::iterator end);

/* THE SECOND (MUNDANE) PART OF THE ALGORITHM:
 * given a fully-built huffman tree, actually make the 
 * mapping between strings and their binary code.
 *
 * We use a queue to explore the tree---recursion would
 * be theoretically nicer, but this is C++.
 */
std::map<std::string,std::string> huffman::build_code(std::shared_ptr<htree> h) {
    typedef std::pair<std::shared_ptr<htree>, std::string> tree_state;
    std::map<std::string, std::string> code;
    std::queue<tree_state> tovisit;
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
