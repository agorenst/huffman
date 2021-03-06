#include "huffman.h"

#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <tuple>

#include <cassert>

#include <memory>

using namespace std;
using namespace huffman;

struct cmp_ptr{
    bool operator()(const shared_ptr<htree>& l, const shared_ptr<htree>& r) {
        return l->w > r->w;
    }
};


shared_ptr<htree> huffman::generate_tree(const std::vector<double>& freqs) {
    if (freqs.size() == 0) { return NULL; } // not sure what else to do, tempted to assert

    priority_queue<shared_ptr<htree>, vector<shared_ptr<htree>>, cmp_ptr> wq;

    // recall the wq really stores shared_ptrs, so this "new" stuff is safe.
    // THIS WAS CHANGED because g++ caused terrible, terrible issues.
    // Maybe I didn't fully understand what I was doing... but even valgrind didn't
    // report issues when running the old code with clang++
    for (unsigned i = 0; i < freqs.size(); ++i) {
        // this IF statement works so long as we don't have rounding errors, haha.
        if (freqs[i] > 0) {
            shared_ptr<htree> to_insert(new htree(freqs[i], i));
            wq.push(to_insert);
        }
    }

    while(wq.size() > 1) {
        auto min1 = wq.top(); wq.pop();
        auto min2 = wq.top(); wq.pop();
        wq.emplace(new htree(min1,min2));
    }

    return wq.top();
}

// we explore the htree in a DFS fashion, always going "left"
// if we can to. KEY FACT: we only go "right" immediately after
// encountering a leaf---thus, when we encounter a leaf, we 
// "pop" back up to the last-visited inner node, and continue from there.
// nodes_since_leaf is sort of like a run-length-encoding.
void huffman::write_htree(const shared_ptr<htree> h, ostream& o) {
    int nodes_since_leaf = 0;
    stack<shared_ptr<htree>> wq;

    wq.push(h);

    while(wq.size() > 0) {
        auto n = wq.top(); wq.pop();
        if (n->is_leaf()) {
            o << nodes_since_leaf << " " << n->i << " ";
            nodes_since_leaf = 0;
        }
        else {
            assert(n->l != NULL && n->r != NULL);
            ++nodes_since_leaf;
            wq.push(n->r);
            wq.push(n->l);
        }
    }
}

// The counterpart to write_htree.
// This is a fairly tricky algorithm, but it is really
// "just" the opposite of write_htree
shared_ptr<htree> huffman::read_htree(std::istream& in) {
    int newnodes, symb;

    shared_ptr<htree> root = NULL;

    // holds all nodes in the tree which have exactly 1
    // child---thus, it is the "incomplete" nodes
    stack<shared_ptr<htree>> wq;

    // the loop invariants:
    // wq contains all vertices of the htree-built-so-far
    // that are both INNER and have ONLY their left-child
    // defined
    // 
    // Each iteration of the loop constructs a new subtree
    // with a root (to_graft), to be grafted onto the top of wq
    for (;;) {
        in >> newnodes;
        in >> symb;


        // if we've triggered EOF after reading, we're done.
        if (in.eof()) {
            assert(wq.size() == 0);
            return root;
        }

        // basically deal with the edge case that
        // we don't have a root yet.
        // if statement will succeed after first iteration.
        shared_ptr<htree> graft_spot = NULL;
        if (wq.size() > 0) {
            graft_spot = wq.top();
            wq.pop();
        }

        // n is the "undefined" node (a root of a subtree)
        // we have memory for it, but it may be inner or a leaf.
        shared_ptr<htree> n(new htree);

        // remember the root of the subtree we're about to build
        shared_ptr<htree> to_graft = n;

        // each go around means n is an inner node!
        for (; newnodes > 0; --newnodes) {
            n->l = shared_ptr<htree>(new htree);
            wq.push(n);
            n = n->l;
        }

        // when we're done, n is the leaf that should hold symb
        n->i = symb;

        if (graft_spot != NULL) {
            graft_spot->r = to_graft;
        }
        else {
            root = to_graft;
        }
    }
}

void huffman::generate_encodings(const shared_ptr<htree> h, vector<string>& encodings) {
    typedef tuple<shared_ptr<htree>, unsigned, bool> tree_state;

    vector<char> codeword(encodings.size());
    stack<tree_state> tovisit;

    // start DFS (we have to do DFS)
    tovisit.push(tree_state(h, 0, false));

    while(tovisit.size()) {
        auto nxt = tovisit.top(); tovisit.pop();
        auto node = get<0>(nxt);
        auto len =  get<1>(nxt);
        auto left = get<2>(nxt);
        if (len > 0) { codeword[len-1] = left ? '0' : '1'; }

        if (node->is_leaf()) {
            encodings[node->i] = string(codeword.begin(), codeword.begin()+len);
        }
        else {
            tovisit.push(tree_state(node->l,len+1,true));
            tovisit.push(tree_state(node->r,len+1,false));
        }
    }
}
