#ifndef  COMPRESS_H_INCLUDED
#define  COMPRESS_H_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <array>
#include <boost/dynamic_bitset.hpp>

typedef std::string bitstring;

std::string dictEncode(std::vector<std::string> dict, std::string input);
std::string BWT(std::string input);
std::string MTFTransform(std::string input, unsigned char d = 2);
bitstring entropyEncode(std::vector<unsigned> freq, std::string input);
bitstring entropyEncodeF(std::vector<unsigned> freq, std::string input);
std::string b64Encode(bitstring input);

std::array<unsigned,256> countBytes(std::string input, std::array<unsigned,256> start = std::array<unsigned,256>());

bitstring b64Decode(std::string input);
std::string entropyDecode(std::vector<unsigned> freq, bitstring input);
std::string entropyDecodeF(std::vector<unsigned> freq, bitstring input);
std::string unMTFTransform(std::string input, unsigned char d = 4);
std::string unBWT(std::string input);
std::string dictDecode(std::vector<std::string> dict, std::string input);

std::vector<bitstring> makeVectorizedCode(std::vector<unsigned> freq);
bitstring fibEncode(unsigned n);

#endif //COMPRESS_H_INCLUDED