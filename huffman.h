#ifndef _NEW_HUFFMAN_H_
#define _NEW_HUFFMAN_H_

#include <string>
#include <vector>
#include <memory>

class enc_tree {
    private:
        shared_ptr<enc_tree> l = NULL;
        shared_ptr<enc_tree> r = NULL;
        const int index = -1;
    public:
        void insert(string s, unsigned i) {
            // if the string is empty, then we should be
            // inserting it into a new node, not an already-existing one.
            assert (s.size() > 0);
            auto choice = *(s.begin());
            assert (choice == '0' || choice == '1');

            // we ``pop'' the string, to give it to our child.
            string next_string(s.begin()+1,s.end());

            // the index should be inserted left or right.
            auto& child = choice == '0' ? l : r;
            if (choice == '0') {
                if (child == NULL) {
                    child = new enc_tree(next_string, i);
                }
                else {
                    child.insert(next_string);
                }
            }
        }
        enc_tree(string& s, unsigned i) {
            if (s == "") {
                l = r = NULL;
                index = i;
            }
            else {
                auto choice = *(s.begin());
                assert (choice == '0' || choice == '1');
                string next_string(s.begin()+1,s.end());
                auto& child = choice == '0' ? l : r;
                child = new enc_tree(next_string, i);
            }
        }
        void lookup(unsigned i, string& s) const {
            if (s.size == 0) {
                assert(i >= 0);
                assert(l == r && l == NULL);
                return i;
            }
            auto choice = *(s.begin());
        }
};

void huffman_encode(const std::vector<double>& freqs, std::vector<std::string>& out);

#endif
