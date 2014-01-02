#include "huffman.h"

#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <tuple>

#include <memory>

using namespace std;

struct htree {
    const shared_ptr<htree> l, r;
    const double w;
    const unsigned i;
    htree(double w, unsigned i): w(w), i(i) {}
    htree(shared_ptr<htree> l, shared_ptr<htree> r):
        l(l), r(r), w(l->w + r->w), i(-1) {}
    bool is_leaf() const { return l == NULL; }
};

struct cmp_ptr{
    bool operator()(const shared_ptr<htree>& l, const shared_ptr<htree>& r) {
        return l->w > r->w;
    }
};

void generate_encodings(const shared_ptr<htree> h, vector<string>& encodings) {
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

void huffman_encode(const vector<double>& freqs, vector<string>& encodings) {
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


    // this generates the encodings
    generate_encodings(wq.top(), encodings);
}
