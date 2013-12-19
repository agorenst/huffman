#include <iostream>

#include "new_huffman.h"

using namespace std;

int main() {
    vector<pair<string,double>> data;
    string s;
    double d;
    for (;;) {
        cin >> s;
        cin >> d;
        if (cin.eof()) { break; }
        data.emplace_back(pair<string,double>(s,d));
    }
    vector<string> words(data.size());
    huffman_encode(data,words);
    for (unsigned i = 0; i < words.size(); ++i) {
        cout << data[i].first << " " << words[i] << endl;
    }
}
