#include <string>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <iostream>

using namespace std;

// this associates a string with a double.
// most importantly, it creates an ordering.
class encoding {
    public:
        const string s; // symbol
        const double f; // frequency of symbol

        encoding(): s(""), f(0) {}
        encoding(string s, double f): s(s), f(f) {}

        // this is important for when we put these in priority queue
        bool operator<(const encoding& e) const {
            if (f == e.f) { return s < e.s; }
            // we go opposite, because priority queue
            // basically, the lowest encoding 
            // is the highest frequency
            return f > e.f;
        }
        encoding operator+(encoding e) const {
            return encoding(s+e.s , f+e.f);
        }
};

ostream& operator<<(ostream& o, const encoding& e) {
    o << e.s << ": " << e.f;
    return o;
}

// this is the simple binary tree structure
// we will be using in our algorithm.
// NOTE: we can only construct either deg-0, or deg-2
// trees! 
struct htree {
    const shared_ptr<htree> l, r;
    const encoding e;
    htree(shared_ptr<htree> l, shared_ptr<htree> r): l(l), r(r), e(l->e + r->e) {}
    htree(encoding e): l(), r(), e(e) {}
    bool is_leaf() const { return l == NULL && r == NULL; }
};

// as we build the forest of more htrees,
// we will be using a lot of shared_ptr<htree>s.
// This makes it more convenient, and MOST IMPORTANTLY,
// adds in the ordering operator, so the priority queue actually
// works correctly.
struct indirect_htree : public shared_ptr<htree> {
    bool operator<(const indirect_htree& h) const {
        return (*this)->e < h->e;
    }
    indirect_htree(htree* h): shared_ptr<htree>(h) {}
};

//THIS IS THE WHOLE ALGORITHM.
// A lot of the "magic" happens thanks to the fact
// that the priority queue works correctly:
// hence (partly) the need for indirect_htree class.
template<class iter>
indirect_htree huffman(iter start, iter end) {
    
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

// so given a huffman encoding (ie, the tree structure),
// now we want to actually print out the encodings.
// Here is the recursive way to do it.
void coding_map(map<string,string>& m, shared_ptr<htree> h, string parent_string, bool left_child) {
    string s = parent_string;
    if (left_child) { s += "0"; }
    else            { s += "1"; }
    if (h->is_leaf()) {
        m[h->e.s] = s;
    }
    else {
        coding_map(m, h->l, s, true);
        coding_map(m, h->r, s, false);
    }
}

// this is the base-case of the recursion.
map<string,string> start_coding_map(shared_ptr<htree> h) {
    map<string,string> m;
    coding_map(m, h->l, "", true);
    coding_map(m, h->r, "", false);
    return m;
}

// here is the infix printing of the htree itself.
ostream& operator<<(ostream& o, shared_ptr<htree> h) {
    if (h == NULL) { return o; }
    o << h->e << endl << h->l << h->r;
    return o;
}

int main(int argc, char* argv[]) {
    std::string s;
    double f;
    std::list<encoding> encodings;
    while(!cin.eof()) {
        cin >> s;
        cin >> f;
        encodings.push_back(encoding(s,f));
        //encodings.insert(encoding(s,f));
    }
    // weird error with input reading loop, so we loop once more.
    // easy remedy.
    encodings.pop_back();

//    for (auto it = encodings.begin(); it != encodings.end(); ++it) {
//        cout << *it << endl;
//    }
    auto h = huffman(encodings.begin(), encodings.end());
    cout << h << endl;
    auto m = start_coding_map(h);
    for (auto it = m.begin(); it != m.end(); ++it) {
        cout << it->first << ": " << it->second << endl;
    }
}
