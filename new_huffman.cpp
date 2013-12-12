#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include <memory>
#include <map>

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


//    queue<shared_ptr<htree>> tovisit;
//    tovisit.push(wq.top());
//    while (tovisit.size()) {
//        auto node = tovisit.front(); tovisit.pop();
//        if (node->is_leaf()) {
//            out << words[node->i] << " " << code << endl;
//        }
//        else {
//            tovisit.push(node->l);
//            tovisit.push(node->r);
//        }
//    }

    typedef pair<shared_ptr<htree>, string> tree_state;

    // we're doing a BFS exploration of h.
    // at each node we save the node location (shared_ptr<htree>)
    // and the currently-built codeword (string), hence tree_state definition
    queue<tree_state> tovisit;

    // start the BFS
    tovisit.push(tree_state(wq.top(),""));
    while(tovisit.size()) {
        auto nxt = tovisit.front(); tovisit.pop();
        auto node = nxt.first;
        auto word = nxt.second;

        if (node->is_leaf()) {
            out << words[node->i] << " " << word << endl;
        }
        else {
            tovisit.push(tree_state(node->l,word+'0'));
            tovisit.push(tree_state(node->r,word+'1'));
        }
    }
}

int main() {
    huffman_encode(cin, cout);
}
