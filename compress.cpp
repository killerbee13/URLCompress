#include <algorithm>
#include <iostream>
#include <queue>
#include <memory>
#include <stdexcept>
#include <unordered_map>

#include "compress.h"
#include "FRC.h"


std::string dictEncode(std::vector<std::string> dict, std::string input)
{
	std::string ret;
	for (size_t i = 0; i < input.size();) {
		//Greedily choose longest matching key in dict
		char best = 0x7F;
		size_t len = 0;
		for (size_t k = 0; k < dict.size(); ++k) {
			if (dict[k].length() > len) {
				if (input.find(dict[k],i) == i) {
					best = k;
					len = dict[k].length();
				}
			}
		}
		if (best == 0x7F) {
			std::cerr<<input[i]<<" not found in dict"<<std::endl;
			// Use '\x7F' as escape character
			ret += best;
			ret += input[i];
			best = 0;
		}
		ret += best;
		// std::cerr<<(int)best<<" // "<<dict[best]<<std::endl;
		//Advance by the correct number of bytes
		i += dict[best].length();
	}
	return ret;
}

std::string dictDecode(std::vector<std::string> dict, std::string input)
{
	std::string ret;
	bool inEscape = false;
	for (auto c : input) {
		if (inEscape) {
			ret += c;
			inEscape = false;
		} else {
			if (c == '\x7F') {
				inEscape = true;
			} else {
				ret += dict[c];
			}
		}
	}
	return ret;
}

std::array<unsigned,256> countBytes(std::string input, std::array<unsigned,256> start)
{
	for (auto c : input) {
		++start[c];
	}
	return start;
}

class string_range
{
public:
	//Default to entire string
	string_range(std::string& u)
		: underlying(u), pos(0), len(u.length()) {;}
	string_range(std::string& u, size_t p, size_t l)
		: underlying(u), pos(p), len(l) {;}
	string_range(const string_range& other)
		: underlying(other.underlying), pos(other.pos), len(other.len) {;}
	string_range& operator=(string_range other)
		{underlying = other.underlying; pos = other.pos; len = other.len; return *this;}
	
	char& operator[](size_t p)
		{return underlying.at(pos+p);}
	const char& operator[](size_t p) const
		{return underlying.at(pos+p);}
	size_t length() const {return len;}
	size_t size() const {return len;}
	bool empty() const {return len>0;}
	
	bool operator<(const string_range& other)
		{return std::lexicographical_compare(
			begin(),end(),
			other.begin(),other.end());}
	bool operator<(const std::string& other)
		{return std::lexicographical_compare(
			begin(),end(),
			other.begin(),other.end());}
		// }} return len<other.length();}
	// bool operator>(const string_range& other)
		// {for (size_t i = 0; i < len && i < other.len; ++i) {
			// if (underlying.at(pos+i) > other[i]) {
				// return 1;
		// }} return len>other.len;}
	// bool operator>(const std::string& other)
		// {for (size_t i = 0; i < len && i < other.length(); ++i) {
			// if (underlying.at(pos+i) > other[i]) {
				// return 1;
		// }} return len>other.length();}
	bool operator==(const string_range& other)
		{for (size_t i = 0; i < len && i < other.len; ++i) {
			if (underlying.at(pos+i) != other[i]) {
				return 0;
		}} return len==other.len;}
	bool operator==(const std::string& other)
		{for (size_t i = 0; i < len && i < other.length(); ++i) {
			if (underlying.at(pos+i) != other[i]) {
				return 0;
		}} return len==other.length();}
	bool operator!=(const string_range& other)
		{return !operator==(other);}
	bool operator!=(const std::string& other)
		{return !operator==(other);}
	// bool operator<=(const string_range& other)
		// {return !operator>(other);}
	// bool operator<=(const std::string& other)
		// {return !operator>(other);}
	bool operator>=(const string_range& other)
		{return !operator<(other);}
	bool operator>=(const std::string& other)
		{return !operator<(other);}
	
	operator std::string()
		{return underlying.substr(pos,len);}
	
	std::string::iterator begin() {return underlying.begin()+pos;}
	const std::string::iterator begin() const {return underlying.begin()+pos;}
	std::string::iterator end() {return underlying.begin()+pos+len;}
	const std::string::iterator end() const {return underlying.begin()+pos+len;}
	
	std::string& underlying;
	size_t pos, len;
};

#define CHAR_EOF ' '

std::string BWT(std::string input)
{
	input = input + CHAR_EOF + input + CHAR_EOF;
	std::vector<string_range> rots(input.length()/2,input);
	for (size_t i = 0; i < rots.size(); ++i) {
		rots[i].pos = i;
		rots[i].len = input.length()/2;
	}
	sort(rots.begin(),rots.end()-1);
	std::string ret;
	for (size_t i = 0; i < rots.size(); ++i) {
		ret += rots[i][rots[i].size()-1];
	}
	return ret;
}

std::string unBWT(std::string input)
{
	std:: string sorted = input;
	sort(sorted.begin(),sorted.end());
	
	std::vector<unsigned> identsLeft(input.length(),0);
	std::map<char,unsigned> charCounts, sortedPos;
	//Guaranteed not to be in input because the character set is 7-bit
	char last = '\xFF';
	size_t I = 0;
	for (size_t i = 0; i != input.length(); ++i) {
		if (charCounts.count(input[i])) {
			++charCounts[input[i]];
		} else {
			charCounts[input[i]] = 0;
		}
		identsLeft[i] = charCounts[input[i]];
		if (sorted[i] != last) {
			sortedPos[sorted[i]] = i;
		}
		if (input[i] == CHAR_EOF) {
			I = i;
		}
	}
	
	std::string ret(input.length(),'\xFF');
	for (size_t p = ret.length(); p > 0; --p) {
		ret[p-1] = input[I];
		std::cerr<<I<<" ("<<(int)input[I]<<" "<<input[I]<<")"<<std::endl;
		size_t l = identsLeft[I], s = sorted.find(input[I],0);
		std::cerr<<l<<" + "<<s<<std::endl;
		I = identsLeft[I] + sorted.find_first_of(input[I]);
	}
	return ret;
}

unsigned char moveToFront(std::vector<char>& alphabet, char c, unsigned char d = 2)
{
	//s = position of c in alphabet, e = position to move c to
	unsigned char s = static_cast<unsigned char>(std::find(alphabet.begin(), alphabet.end(), c)-alphabet.begin()),
		e = s/d;
	if ((!s) || d==1) {
		return s;
	}
	for (unsigned char p = s; p > e; --p) {
		alphabet[p] = alphabet[p-1];
	}
	alphabet[e] = c;
	return s;
}

std::string MTFTransform(std::string input, unsigned char d)
{
	std::string ret;
	std::vector<char> alphabet(128,'\0');
	//alphabet starts as the ordered range over the symbol set
	std::iota(alphabet.begin(), alphabet.end(), 0);
	for (auto c : input) {
		ret.push_back(moveToFront(alphabet,c,d));
	}
	return ret;
}

std::string unMTFTransform(std::string input, unsigned char d)
{
	std::string ret;
	std::vector<char> alphabet(128,'\0');
	//alphabet starts as the ordered range over the symbol set
	std::iota(alphabet.begin(), alphabet.end(), 0);
	for (auto c : input) {
		ret.push_back(alphabet[c]);
		moveToFront(alphabet,c,d);
	}
	return ret;
}

struct BinaryTreeNode
{
	BinaryTreeNode(bool i, unsigned f, char v, std::shared_ptr<BinaryTreeNode> z, std::shared_ptr<BinaryTreeNode> o)
		: internal(i), freq(f), val(v), zero(z), one(o) {;}
	bool internal;
	unsigned freq;
	char val;
	std::shared_ptr<BinaryTreeNode> zero, one;
	std::weak_ptr<BinaryTreeNode> parent;
	// ~BinaryTreeNode() {std::cerr<<escapify(val);}
};

std::shared_ptr<BinaryTreeNode> makeHuffmanCode(std::vector<unsigned> freq)
{
	std::queue<std::shared_ptr<BinaryTreeNode>> q1, q2;
	// For all chars in reverse order:
	unsigned char c = freq.size();
	while (c--) {
		q1.push(std::make_shared<BinaryTreeNode>(false, freq[c]+1, c, nullptr, nullptr));
	}
	while (q1.size() + q2.size() > 1) {
		std::shared_ptr<BinaryTreeNode> t1, t2;
		if (q1.size() && q2.size()) {
			if (q1.front()->freq <= q2.front()->freq) {
				t1 = q1.front();
				q1.pop();
			} else {
				t1 = q2.front();
				q2.pop();
			}
			if (q1.size() && q2.size()) {
				if (q1.front()->freq <= q2.front()->freq) {
					t2 = q1.front();
					q1.pop();
				} else {
					t2 = q2.front();
					q2.pop();
				}
			} else if (q1.size()) {
				t2 = q1.front();
				q1.pop();
			} else if (q2.size()) {
				t2 = q2.front();
				q2.pop();
			}
		} else if (q1.size() > 1) {
			t1 = q1.front();
			q1.pop();
			t2 = q1.front();
			q1.pop();
		} else if (q2.size() > 1) {
			t1 = q2.front();
			q2.pop();
			t2 = q2.front();
			q2.pop();
		}
		auto tp = std::make_shared<BinaryTreeNode>(true,(t1->freq + t2->freq), 0, t1, t2);
		q2.push(tp);
		t1->parent = tp;
		t2->parent = tp;
	}
	if (q1.size() + q2.size() > 1) {
		throw std::logic_error(std::string("Queues not empty:") + toStr(q1.size())+" "+toStr(q2.size()));
	}
	return q2.front();
}

std::vector<bitstring> vectorizeCode(std::shared_ptr<BinaryTreeNode> root)
{
	//Save root so that the tree isn't deleted
	std::shared_ptr<BinaryTreeNode> current = root;
	unsigned char total = 128;
	std::vector<bitstring> codes(total);
	bitstring word;
	unsigned char count = 0;
	while (count < total) {
		// std::cerr<<word<<" "<<(int)count<<std::endl;
		//If internal node
		if (current->internal) {
			word.push_back('0');
			current = current->zero;
		} else {
			if (codes[current->val].size()) {
				throw std::logic_error("Hit same word twice?");
			}
			codes[current->val] = word;
			++count;
			if (!current->parent.lock()) {
				throw std::logic_error("Root is leaf");
			}
			while (current == current->parent.lock()->one) {
				current = current->parent.lock();
				word.pop_back();
				if (!current->parent.lock()) {
					return codes;
				}
			}
			current = current->parent.lock()->one;
			word.pop_back();
			word.push_back('1');
		}
	}
	// std::cerr<<"Tree can be deleted now"<<std::endl;
	return codes;
}

std::vector<bitstring> makeVectorizedCode(std::vector<unsigned> freq)
{
	return vectorizeCode(makeHuffmanCode(freq));
}

bitstring fibEncode(unsigned n)
{
	if (n == 0) {
		throw std::domain_error("There is no Fibonacci code for zero.");
	} if (n == 1) {
		return "11";
	} else if (n == 2) {
		return "011";
	} else if (n == 3) {
		return "0011";
	}
	unsigned i = 2;
	while (fibonacci(i+1) <= n) {
		++i;
	}
	bitstring ret(i,'0');
	ret[i-1] = '1';
	while (n) {
		// std::cerr<<"n="<<n<<std::endl;
		// std::cerr<<"i="<<i<<" f(i)="<<fibonacci(i)<<std::endl;
		if (fibonacci(i) <= n) {
			n -= fibonacci(i);
			ret[i-2] = '1';
		}
		--i;
	}
	return ret;
}

bitstring entropyEncode(std::vector<unsigned> freq, std::string input)
{
	std::vector<bitstring> outSet = vectorizeCode(makeHuffmanCode(freq));
	bitstring ret = fibEncode(input.length());
	// std::cerr<<"length: "<<input.length()<<" f: "<<ret<<std::endl;
	for (unsigned char c : input) {
		ret += outSet[c];
	}
	return ret;
}

bitstring entropyEncodeF(std::vector<unsigned> freq, std::string input)
{
	bitstring ret;
	// std::cerr<<"length: "<<input.length()<<" f: "<<ret<<std::endl;
	for (unsigned char c : input) {
		ret += fibEncode((+c)+1);
	}
	return ret;
}

std::string entropyDecode(std::vector<unsigned> freq, bitstring input)
{
	auto root = makeHuffmanCode(freq), current = root;
	bool isCompleteWord = false, hasLength = false, lastWasOne = false;
	unsigned length = 0;
	std::string ret;
	for (size_t p = 0; p != input.size(); ++p) {
		if (!hasLength) {
			if (input[p] == '1') {
				if (lastWasOne) {
					hasLength = true;
				} else {
					length += fibonacci(p+2);
					lastWasOne = true;
				}
			}
		} else {
			
		}
	}
	return std::string("");
}

std::string entropyDecodeF(std::vector<unsigned> freq, bitstring input)
{
	bool lastWasOne = false;
	unsigned char word = 0;
	std::string ret;
	for (size_t p = 0; p != input.size(); ++p) {
		if (input[p] == '1') {
			if (lastWasOne) {
				ret += (char)word;
			} else {
				word += fibonacci(p+2);
				lastWasOne = true;
			}
		}
	}
	return ret;
}

// Custom ASCII-sortable base 64 set
const std::string b64(
	"01234567"
	"89ABCDEF"
	"GHIJKLMN"
	"OPQRSTUV"
	"WXYZ_abc"
	"defghijk"
	"lmnopqrs"
	"tuvwxyz~"
);

std::string b64Encode(bitstring input)
{
	// Resize input to next multiple of 6
	while (input.size()%6) {
		input += '0';
	}
	// input.append('0',(6-input.size()%6)%6);
	string_range group6(input,0,6);
	std::string ret;
	for (size_t i = 0; i < input.length(); i+=6) {
		size_t index = 0;
		group6.pos = i;
		for (size_t j = 0; j != 6; ++j) {
			if (group6[j] == '1') {
				index += 1<<j;
			}
		}
		// std::cerr<<"got: "<<index<<" :"<<b64[index]<<std::endl;
		ret += b64[index];
	}
	return ret;
}

#include "b64d.h"

bitstring b64Decode(std::string input)
{
	bitstring ret;
	for (auto c : input) {
		if (b64D.count(c)) {
			ret += b64D.at(c);
		} else {
			//Error in input
		}
	}
	return ret;
}