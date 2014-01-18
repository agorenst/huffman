#include "huffman.h"

#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <tuple>

#include<cassert>

#include <memory>

using namespace std;

struct cmp_ptr{
    bool operator()(const shared_ptr<htree>& l, const shared_ptr<htree>& r) {
        return l->w > r->w;
    }
};

shared_ptr<htree> generate_tree(const std::vector<double>& freqs) {
    if (freqs.size() == 0) { return NULL; } // not sure what else to do, tempted to assert

    priority_queue<shared_ptr<htree>, vector<shared_ptr<htree>>, cmp_ptr> wq;

    // recall the wq really stores shared_ptrs, so this "new" stuff is safe.
    for (auto i = 0; i < freqs.size(); ++i) {
        wq.emplace(new htree(freqs[i], i));
    }

    while(wq.size() > 1) {
        auto min1 = wq.top(); wq.pop();
        auto min2 = wq.top(); wq.pop();
        wq.emplace(new htree(min1,min2));
    }

    return wq.top();
}

void write_htree(const shared_ptr<htree> h, ostream& o) {
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

// this builds an htree, but doesn't fill in the weights (w).
// so be wary?
shared_ptr<htree> read_htree(std::istream& in) {
    int newnodes, symb;

    shared_ptr<htree> root = NULL;

    // holds all nodes in the tree which have exactly 1
    // child---thus, it is the "incomplete" nodes
    stack<shared_ptr<htree>> wq;

    for (;;) {
        in >> newnodes;
        in >> symb;


        // if we've triggered EOF after reading, we're done.
        if (in.eof()) {
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

        // n is the "undefined" node
        // we have memory for it, but it may be inner or a leaf.
        shared_ptr<htree> n(new htree);
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
            cerr << "GRAFTING ROOT" << endl;
            root = to_graft;
        }
    }
}
