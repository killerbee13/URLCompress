#include <stdexcept>
#include "countertrie.h"

void CounterTrie::insert_with_prefixes(std::string key)
{
	if (key.empty())
		return;
	unsigned int p = 0, i = 0;
	for (; i != key.length() - 1; ++i) {
		// key[i] = (key[i]-0x21)%95;
		if (index[p][(key[i]-0x21)%95].first == 0) {
			index[p][(key[i]-0x21)%95].first = index.size();
			index.push_back(mapNull());
		}
		++index[p][(key[i]-0x21)%95].second;
		p = index[p][(key[i]-0x21)%95].first;
	}
}

void CounterTrie::insert(std::string key)
{
	if (key.empty())
		return;
	unsigned int p = 0, i = 0;
	for (; i != key.length() - 1; ++i) {
		// key[i] = (key[i]-0x21)%95;
		if (index[p][(key[i]-0x21)%95].first == 0) {
			index[p][(key[i]-0x21)%95].first = index.size();
			index.push_back(mapNull());
		}
		p = index[p][(key[i]-0x21)%95].first;
	}
	++index[p][(key[i]-0x21)%95].second;
}

size_t CounterTrie::count(std::string key)
{
	if (key.empty())
		return 0;
	unsigned int p = 0, i = 0;
	for (; i != key.length() - 1; ++i) {
		// key[i] = (key[i]-0x21)%95;
		if (index[p][(key[i]-0x21)%95].first == 0) {
			return 0;
		}
		p = index[p][(key[i]-0x21)%95].first;
	}
	return index[p][(key[i]-0x21)%95].second;
}

int& CounterTrie::operator[](std::string key)
{
	if (key.empty())
		throw std::invalid_argument("Empty string cannot be a member of trie");
	unsigned int p = 0, i = 0;
	for (; i != key.length() - 1; ++i) {
		// key[i] = (key[i]-0x21)%95;
		if (index[p][(key[i]-0x21)%95].first == 0) {
			index[p][(key[i]-0x21)%95].first = index.size();
			index.push_back(mapNull());
		}
		p = index[p][(key[i]-0x21)%95].first;
	}
	return index[p][(key[i]-0x21)%95].second;
}

//Find all (valid) terminal nodes downstream from key
std::vector<std::string> CounterTrie::allWithPrefix(std::string key)
{
	if (key.empty())
		return i_getNodes(0, key);
	// std::string okey = key;
	unsigned int p = 0, i = 0;
	//Stop one char early and finish outside loop
	for (; i != key.length() - 1; ++i) {
		// key[i] = (key[i]-0x21)%95;
		if (index[p][(key[i]-0x21)%95].first == 0) {
			return std::vector<std::string>(); //key is not a prefix in trie
		}
		p = index[p][(key[i]-0x21)%95].first;
	}
	std::vector<std::string> tmp;
	//If key is a valid node, add it
	if (index[p][(key[i]-0x21)%95].second) 
		tmp.push_back(key);
	//If key has no children, stop
	if (!index[p][(key[i]-0x21)%95].first) {
		return tmp;
	}
	p = index[p][(key[i]-0x21)%95].first;
	auto tmp2 = i_getNodes(p, key);
	tmp.insert(tmp.end(), tmp2.begin(), tmp2.end());
	return tmp;
}

//Get descendents of node
std::vector<std::string> CounterTrie::i_getNodes(unsigned int p, std::string prefix)
{
	//Depth-first (recursive) search
	std::vector<std::string> retArray;
	for (unsigned char j = 0; j != 95; ++j) {
		if (index[p][j].second) { //Terminal node, add to output
			retArray.push_back(prefix + static_cast<char>(j+0x21));
		}
		if (index[p][j].first) { //Node has children, recurse
			auto tmp = i_getNodes(index[p][j].first, prefix + static_cast<char>(j+0x21));
			retArray.insert(retArray.end(), tmp.begin(), tmp.end());
		}
	}
	return retArray;
}
