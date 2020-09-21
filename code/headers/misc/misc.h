#ifndef RANDOMG_H
#define RANDOMG_H
#include <string>
#include <fstream>
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

template <typename T>
class SmArray {
private:
	T* mem;
	unsigned int Num;
	unsigned max_size;
public:

	T& at(unsigned int i) {
		//if (i + 1 > current_size)
		//	assert(0);
		return mem[i];
	}

	T& operator[] (unsigned int index) {
		return at(index);
	}

	void resize(unsigned int news) {
		Num = news;
	}

	void clear() {
		Num = 0;
	}

	void push_back(T& item) {
		if (Num < max_size) {
			mem[Num] = item;
			Num++;
		}
	}

	void push_back_Unique(T& item) {
		if (!isInList(item))
			push_back(item);
	}

	void pop_back() {
		if (Num > 0)
			Num--;
	}

	bool isInList(T& item) {
		for (unsigned int i = 0; i < max_size; i++) { //Iterating over all objects
			if (mem[i] == item) { //If object marked as deleted
				return true;
			}
		}
		return false;
	}

	void _delete(T& item) {
		for (unsigned int i = 0; i < Num; i++) { //Iterating over all objects
			if (mem[i] == item) { //If object marked as deleted
				for (unsigned int obj_i = i + 1; obj_i <= Num; obj_i++) { //Iterate over all next chidren
					mem[obj_i - 1] = mem[obj_i]; //Move it to previous place
				}
				pop_back(); //remove last position
			}
		}
	}

	unsigned int size() {
		return Num;
	}

	void reserve(unsigned int size) {
		mem = new T[size];
		max_size = size;
	}
	SmArray() {
		Num = 0;
	}
	SmArray(unsigned int size) {
		reserve(size);
		Num = 0;
	}
	~SmArray() {
		delete[] mem;
	}
};

#endif // RANDOMG_H
