#ifndef _NEW_HUFFMAN_H_
#define _NEW_HUFFMAN_H_

#include <string>
#include <vector>

void huffman_encode(const std::vector<std::pair<std::string,double>>& freqs, std::vector<std::string>& out);

#endif
