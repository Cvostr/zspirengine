#ifndef RANDOMG_H
#define RANDOMG_H
#include <string>
void genRandomString(std::string* result, unsigned int len);
void readString(std::string& str, const char* data, unsigned int& offset);
template <typename T>
void readBinaryValue(T* out, const char* data, unsigned int& offset) {
	memcpy(out, data, sizeof(T));
	offset += sizeof(T);
}
#endif // RANDOMG_H
