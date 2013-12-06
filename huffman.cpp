
#include <string>
#include <map>

#include <list>
#include <memory> // shared_ptr
#include <queue> // priority_queue
#include <iostream>
#include <cmath>

using namespace std;

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
    const shared_ptr<htree> l, r;
    const encoding e;
    htree(shared_ptr<htree> l, shared_ptr<htree> r): l(l), r(r), e(l->e + r->e) {}
    htree(encoding e): l(), r(), e(e) {}
    bool is_leaf() const { return l == NULL && r == NULL; }
};


// the huffman algorithm has to order the htrees
// based on their "weight".
// Moreover, it only makes sense to work with shared_ptr<htree>s.
// This provides a neat wrapper for both.
struct indirect_htree : public shared_ptr<htree> {
    bool operator<(const indirect_htree& h) const {
        return (*this)->e < h->e;
    }
    indirect_htree(htree* h): shared_ptr<htree>(h) {}
};

/* THE MAIN ALGORITHM */

// A lot of the "magic" happens thanks to the fact
// that the priority queue works correctly:
// hence (partly) the need for indirect_htree class.
template<class iter>
shared_ptr<htree> build_tree(iter start, iter end) {

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

/* THE SECOND (MUNDANE) PART OF THE ALGORITHM:
 * given a fully-built huffman tree, actually make the 
 * mapping between strings and their binary code.
 *
 * We use a queue to explore the tree---recursion would
 * be theoretically nicer, but this is C++.
 */
map<string,string> build_code(shared_ptr<htree> h) {
    map<string, string> code;
    std::queue<pair<shared_ptr<htree>,string>> tovisit;
    tovisit.push(pair<shared_ptr<htree>,string>(h,""));
    while(tovisit.size()) {
        auto nxt = tovisit.front(); tovisit.pop();
        auto node = nxt.first;
        auto word = nxt.second;
        if (node->is_leaf()) {
            code[node->e.first] = word;
        }
        else {
            tovisit.push(pair<shared_ptr<htree>,string>(node->l,word+'0'));
            tovisit.push(pair<shared_ptr<htree>,string>(node->r,word+'1'));
        }
    }
    return code;
}

template<class iter>
double compute_cost(iter begin, iter end, map<string,string>& code) {
    double cost = 0;
    for (auto it = begin; it != end; ++it) {
        cost += code[it->first].size()*(it->second);
    }
    return cost;
}

template<class iter>
double compute_entropy(iter begin, iter end) {
    double cost = 0;
    for (auto it = begin; it != end; ++it) {
        double freq = it->second;
        cost += -(log(freq)/log(2))*freq;
    }
    return cost;
}

/*
 * STARTING PRINTING FUNCTIONS
 */
ostream& operator<<(ostream& o, const encoding& e) {
    o << e.first << ": " << e.second;
    return o;
}
// here is the infix printing of the htree itself.
ostream& operator<<(ostream& o, shared_ptr<htree> h) {
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
    list<encoding> encodings;
    while(!cin.eof()) {
        cin >> s;
        cin >> f;
        encodings.push_back(encoding(s,f));
        //encodings.insert(encoding(s,f));
    }
    // weird error with input reading loop, so we loop once more.
    // easy remedy. TODO how set eof before failure? I see what's going on...
    encodings.pop_back();

    // from the basic encodings, build the huffman tree.
    auto h = build_tree(encodings.begin(), encodings.end());

    cout << h << endl;
    // recursively traverse the huffman tree to build the encoding.
    //auto m = start_coding_map(h);
    auto m = build_code(h);
//
//    // sending encoding to stdout.
    for (auto it = m.begin(); it != m.end(); ++it) {
        cout << it->first << ": " << it->second << endl;
    }
    cout << "Cost: " << compute_cost(encodings.begin(), encodings.end(), m) << endl;
    cout << "Entropy: " << compute_entropy(encodings.begin(), encodings.end()) << endl;
}
