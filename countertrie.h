#ifndef  COUNTER_TRIE_H_INCLUDED
#define  COUNTER_TRIE_H_INCLUDED

#include <vector>
#include <array>
#include <functional>
#include <algorithm>
#include <memory>
#include <cstdint>

//implements an indexed counter trie
class CounterTrie
{
public:
	CounterTrie() : index(1) {;}
	void insert_with_prefixes(std::string key);
	void insert(std::string key);
	int& operator[](std::string key);
	size_t count(std::string key);
	std::string next(std::string key);
	std::vector<std::string> allWithPrefix(std::string key);
	std::vector<std::string> i_getNodes(unsigned int p, std::string prefix);
private:
	constexpr static std::array<std::pair<int, int>, 95> mapNull()
		{return std::array<std::pair<int, int>, 95>();};
	std::vector<std::array<std::pair<int, int>, 95>> index;
};

#endif //COUNTER_TRIE_H_INCLUDED
