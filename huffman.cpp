#include <string>
#include <map>
#include <memory>
#include <queue>
#include <sstream>
#include <fstream>
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


class htree {
    private:
        // a recursive tree, has an encoding and 2 children.
        // either has both children, or none
        class hnode {
            public:
                const shared_ptr<hnode> l, r;
                const encoding e;

                hnode() {}
                hnode(shared_ptr<hnode> l, shared_ptr<hnode> r): l(l), r(r), e(l->e + r->e) {}
                hnode(encoding e): l(), r(), e(e) {}
                bool is_leaf() const { return l == NULL && r == NULL; }
                
                // this isn't printing, but rather saving our binary string mapping to the
                // map.
                void print_code(map<string, string>& m, const string parentcode, bool left) {
                    string code = parentcode;
                    if (left) { code += "0"; }
                    else { code += "1"; }
                    if (is_leaf()) {
                        m[e.s] = code;
                    }
                    else {
                        l->print_code(m, code, true);
                        r->print_code(m, code, false);
                    }
                }
        };

    public:
        // htree basically "kicks off" the recursive tree structure
        // expressed by the hnodes
        //
        // by using shared pointers instead of regular ones,
        // no explicit deletes are needed.
        shared_ptr<hnode> me;

        htree() {}
        htree(shared_ptr<hnode> me): me(me) {}
        htree(htree l, htree r): me(new hnode(l.me, r.me)) {}
        htree(encoding e): me(new hnode(e)) {}
        bool is_leaf() const { return me->is_leaf(); }

        bool operator<(const htree& h) const {
            return me->e < h.me->e;
        }

        map<string, string> encode() {
            map<string, string> m;
            me->l->print_code(m, "", true);
            me->r->print_code(m, "", false);
            return m;
        }
};

ostream& operator<<(ostream& o, htree h) {
    if (h.me == NULL) { return o; }
    return (o << h.me->e << endl << htree(h.me->l) << htree(h.me->r));
}

// this takes in a frequency mapping, and does the greedy algorithm
// to build the final huffman tree.
class huffman {
    public:
        htree code;
        huffman(map<string, float> m) {
            priority_queue<htree> wq; // my workqueue
            for (auto it = m.begin(); it != m.end(); ++it) {
                wq.push(htree(encoding(it->first, it->second)));
            }

            // this for loop instead of the while loop more
            // explicitly says the runtime.
            //int n = wq.size();
            //for (int k = n+1; k < 2*n; ++k) {
            // now we do the loop:
            while (wq.size() > 1) {
                auto min1 = wq.top(); wq.pop();
                auto min2 = wq.top(); wq.pop();
                wq.push(htree(min1,min2));
            }
            code = wq.top();
        }
};

map<string, float> loadencs(istream& infile) {
    map<string, float> contents;
    string word, line;
    double freq;
    while (getline(infile, line)) {
        istringstream ss(line);
        ss >> word;
        ss >> freq;
        contents[word] = freq;
    }
    return contents;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "usage: ./huffman <filename>\n";
        return 0;
    }
    ifstream file;
    file.open(argv[1]);

    map<string, float> contents = loadencs(file);
    file.close();

    huffman h(contents);
    cout << h.code << endl;
    auto m = h.code.encode();
    for (auto i = m.begin(); i != m.end(); ++i) {
        cout << i->first << ": " << i->second << endl;
    }
}
