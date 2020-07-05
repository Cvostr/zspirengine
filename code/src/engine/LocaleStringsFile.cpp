#include "../../headers/engine/LocaleStringsFile.h"
#include <fstream>
#include <iostream>

using namespace ZSPIRE;
using namespace std;

LocaleStringsFile::LocaleStringsFile(char* data) {
	StringsCount = 0;
	langsCount = 0;
	//Read from buffer
	readFileHeader(data);
}
LocaleStringsFile::LocaleStringsFile(std::string file_path) {
	StringsCount = 0;
	langsCount = 0;

	ifstream stream;
	stream.open(file_path, std::ifstream::binary | std::ifstream::ate);
	unsigned int size = stream.tellg();
	stream.seekg(0);

	char* data = new char[size];
	//Read file
	stream.read(data, size);
	//Read from buffer
	readFileHeader(data);
}
LocaleStringsFile::~LocaleStringsFile() {

}
bool LocaleStringsFile::readFileHeader(char* data) {
	char header[25];
	header[24] = '\0';
	//Read header
	memcpy(header, data, 24);
	if (strcmp(header, "ZSLOCALIZEDSTRINGSBINARY")) {
		return false;
	}
	//Read Stats
	memcpy(&this->StringsCount, data + 24, sizeof(unsigned int));
	memcpy(&this->langsCount, data + 28, sizeof(unsigned int));

	return true;
}

unsigned int LocaleStringsFile::getLanguagesCount() {
	return this->langsCount;
}
unsigned int LocaleStringsFile::getStringsCount() {
	return this->StringsCount;
}
void LocaleStringsFile::pushLanguageCodeStr(std::string str) {
	lang_codes[langsCount] = str;
	langsCount++;
}