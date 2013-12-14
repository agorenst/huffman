#include "new_huffman.h"

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

//shared_ptr<htree> build_tree

void huffman_encode(istream& in, ostream& out) {
    vector<string> words;
    priority_queue<shared_ptr<htree>, vector<shared_ptr<htree>>, cmp_ptr> wq;

    string s;
    double d;
    for (;;) {
        in >> s;
        in >> d;
        if (in.eof()) { break; }


        // we build a static ``array'' of words and our initial workqueue
        words.emplace_back(s);
        wq.emplace(new htree(d, words.size()-1));
    }

    while(wq.size() > 1) {
        auto min1 = wq.top(); wq.pop();
        auto min2 = wq.top(); wq.pop();
        wq.emplace(new htree(min1,min2));
    }


    // definitely more room than we need---shrink?
    vector<char> codeword(words.size());
    typedef tuple<shared_ptr<htree>, unsigned, bool> tree_state;

    stack<tree_state> tovisit;

    // start DFS (we have to do DFS)
    tovisit.push(tree_state(wq.top(),0, false));

    while(tovisit.size()) {
        auto nxt = tovisit.top(); tovisit.pop();
        auto node = get<0>(nxt);
        auto len =  get<1>(nxt);
        auto left = get<2>(nxt);
        if (len > 0) { codeword[len-1] = left ? '0' : '1'; }

        if (node->is_leaf()) {
            out << words[node->i] << " ";
            for (unsigned l = 0; l < len; ++l) {
                out << codeword[l];
            }
            out << endl;
        }
        else {
            tovisit.push(tree_state(node->l,len+1,true));
            tovisit.push(tree_state(node->r,len+1,false));
        }
    }
}
