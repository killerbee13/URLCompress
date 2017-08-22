#ifndef  SIMPLE_ALGORITHMS_H_INCLUDED
#define  SIMPLE_ALGORITHMS_H_INCLUDED

//Header containing frequently reused code that is not as easily done without helper functions

#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <istream>
#include <cctype>
#include <utility>
#include <typeinfo>
#include <iomanip>
#include <stdexcept>

//This is the only one that can be expected to throw an exception
template <typename T>
inline T fromStr(std::string val) {
	std::stringstream ss(val);
	T ret;
	if (!(ss>>ret).fail())
		return ret;
	else throw std::runtime_error("\""+val+"\" is not a "+typeid(T).name());
}

//Remember std::to_string() in <string> for builtin types
template <typename T>
inline std::string toStr(T val) {
	std::stringstream ss;
	ss<<val;
	return ss.str();
}

inline std::string reverseStr(std::string val) {
	std::reverse(val.begin(), val.end());
	return val;
}

inline std::string toLower(std::string str) {
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

inline std::string toUpper(std::string str) {
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	return str;
}

template <typename T>
inline T arraycat(T A, T B)
{
	A.insert(A.end(), B.begin(), B.end());
	return A;
}

template <typename T>
inline std::vector<T> listToVec(std::string list)
{
	std::vector<T> ret;
	std::stringstream ss(list);
	std::string tmp;
	while(!(ss>>tmp).fail()) {
		ret.push_back(fromStr<T>(tmp));
	}
	/*
	while(list.find_first_of(" ,") != std::string::npos) {
		std::string entry = list.substr(0, list.find_first_of(" ,"));
		ret.push_back(fromStr<T>(entry));
		list = list.substr(entry.size());
		if (list.find_first_of(" ,") == std::string::npos) {
			entry = list.substr(0, list.find_first_of(" ,"));
			ret.push_back(fromStr<T>(entry));
			list = list.substr(entry.size());
		}
	}*/
	return ret;
}

inline std::string repeat(std::string val, int count) {
	std::string tmp;
	for (int i = 0; i < count; ++i) {
		tmp += val;
	}
	return tmp;
}

inline std::string repeat(char val, int count) {
	std::string tmp;
	for (int i = 0; i < count; ++i) {
		tmp += val;
	}
	return tmp;
}

template<typename Number>
constexpr inline int digitsOf(Number val)
{
	return std::floor(std::log10(val))+1;
}

template<typename Number>
constexpr inline int digitsOf(Number val, int base)
{
	return std::floor(std::log(val)/std::log(base))+1;
}

template<typename ForwardIterator>
inline int digitsList(ForwardIterator first, ForwardIterator last)
{
	return digitsOf(*std::max_element(first, last));
}

template<typename ForwardIterator>
inline int digitsList(ForwardIterator first, ForwardIterator last, int base)
{
	return digitsOf(*std::max_element(first, last), base);
}

//Not working
template<typename ostream_type, typename ForwardIterator>
inline ostream_type& padList(ostream_type& os, ForwardIterator first,
			ForwardIterator last, char fill=' ', int base=10)
{
//os<<std::setw(std::ceil(std::log(*std::max_element(first, last))/std::log(base)))<<std::setfill(fill);
	
	//save state of os
	char fills = os.fill(); std::streamsize widths = os.width();
	
	os<<std::setw(digitsList(first, last, base))<<std::setfill(fill);
	
	for (ForwardIterator it = first; it != last; ++it) {
		os<<' '<<*it;
	}
	//restore state of os
	os<<std::setw(widths)<<std::setfill(fills);
	return os;
}

//Consume all non-spaces to first break, then eat that, too
inline std::istream& eatWord(std::istream& is)
{
	do {is.get();} while (!isspace(is.peek()));
	return is;
}

//Eat spaces, don't eat an extra
inline std::istream& eatSpace(std::istream& is)
{
	while (isspace(is.peek())) is.get();
	return is;
}

inline std::string url_encode(const std::string &value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        std::string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << int((unsigned char) c);
        escaped << std::nouppercase;
    }

    return escaped.str();
}

inline std::string get_file_contents(const char *filename)
{
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in) {
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

//Accepts any sequence of char, returns printable string
template <typename T>
inline std::string escapify(T value)
{
	std::ostringstream ret;
	for (char c : value) {
		if (c < ' ' || c == '\x7F' || c & '\x80') {
			ret<<"\\x"<<std::hex<<std::setfill('0')<<std::setw(2)
				<<(int)c
				<<std::dec;
		} else {
			ret<<c;
		}
	}
	return ret.str();
}

template <>
inline std::string escapify<char>(char value)
{
	
	if (value < ' ' || value == '\x7F' || value & '\x80') {
		static const char* digits = "0123456789ABCDEF";
		std::string rc("\\x  ");
		rc[2] = digits[value&15];
		rc[3] = digits[value>>4];
		return rc;
	} else {
		return std::string(1,value);
	}
}

// Semi-efficient fibonacci implementation (constant for small n)
inline unsigned fibonacci(unsigned n) {
	constexpr std::array<unsigned,39> fibs
		{0,1,1,2,3,5,8,13,21,34,55,89,144,233,377,610,987,
		 1597,2584,4181,6765,10946,17711,28657,46368,75025,
		 121393,196418,317811,514229,832040,1346269,
		 2178309,3524578,5702887,9227465,14930352,24157817,
		 39088169};
	if (n < fibs.size()) {
		return fibs[n];
	// Iterative fallback
	} else {
		unsigned b1=fibs[fibs.size()-2], b2=fibs[fibs.size()-1], t=b1+b2;
		for (unsigned i = fibs.size(); i != n; ++i) {
			t = b2+b1;
			b2 = b1;
			b1 = t;
		}
		return t;
		// if (n > 1000) {
			
			// throw std::overflow_error(std::string("fibonacci(n>1000) will probably segfault. n=")+toStr(n));
		// }
		// return fibonacci(n-1) + fibonacci(n-2);
	}
}

//Similar to std::pair
template <typename T>
class ordered_pair {
using pair=std::pair<T, T>;
public:
	ordered_pair(T a, T b)  { if (a < b) {first = a; second = b;}
														else {first = b; second = a;} }
	T first, second;
	operator pair() {return std::pair<T, T>(first, second);}
	bool operator < (const ordered_pair& rhs) const
	{
		if (first < rhs.first) return 1;
		if (second < rhs.second) return 1;
		return 0;
	}
};

template <typename T>
ordered_pair<T> mkop(T a, T b) 
{
	return ordered_pair<T>(a, b);
}

//Memnonic aids

template <typename T>
using Ptr = T*;

template <typename T>
using PtrToConst = T const*;

template <typename T>
using ConstPtr = T* const;

template <typename T>
using ConstPtrToConst = T const* const;

#endif //SIMPLE_ALGORITHMS_H_INCLUDED
