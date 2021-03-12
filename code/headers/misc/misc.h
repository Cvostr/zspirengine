#pragma once 

#include <string>
#include <fstream>
#include <cstring>
#include <math/Vec3.hpp>
#include <math/Vec2.hpp>
#include <SDL2/SDL.h>

void genRandomString(std::string* result, unsigned int len);
void readString(std::string& str, const char* data, unsigned int& offset);
void readLine(std::string& str, const char* data, unsigned int& offset);
void skipSpaces(const char* data, unsigned int& offset);
template <typename T>
void readBinaryValue(T* out, const char* data, unsigned int& offset) {
	memcpy(out, data, sizeof(T));
	offset += sizeof(T);
}

template <typename T>
T GetRandomValue(T min, T max) {
	srand(static_cast<unsigned int>(SDL_GetPerformanceCounter()));

	T v = min + rand() % (int)(max - min);
	return v;
}

void ProcessFileName(std::string& str);

bool startsWith(std::string& s, std::string m);

void RemoveExtension(std::string& str);
void GetFileName(std::string& str);

class ZsStream : public std::ofstream {
public:
	template <typename T>
	void writeBinaryValue(T* v) {
		write(reinterpret_cast<const char*>(v), sizeof(T));
	}

	void writeVec3(Vec3& vec) {
		write(reinterpret_cast<char*>(&vec), sizeof(float) * 3);
	}

	void writeVec2(Vec2& vec) {
		write(reinterpret_cast<char*>(&vec), sizeof(float) * 2);
	}

	void writeString(std::string str);
};