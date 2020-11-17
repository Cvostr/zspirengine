#pragma once 

#include <string>
#include <fstream>
#include <cstring>

void genRandomString(std::string* result, unsigned int len);
void readString(std::string& str, const char* data, unsigned int& offset);
void readLine(std::string& str, const char* data, unsigned int& offset);
void skipSpaces(const char* data, unsigned int& offset);
template <typename T>
void readBinaryValue(T* out, const char* data, unsigned int& offset) {
	memcpy(out, data, sizeof(T));
	offset += sizeof(T);
}

bool startsWith(std::string& s, std::string m);

class ZsStream : public std::ofstream {
public:
	template <typename T>
	void writeBinaryValue(T* v) {
		write(reinterpret_cast<const char*>(v), sizeof(T));
	}

	void writeString(std::string str);
};