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
	memcpy(&this->StringsCount, data + 25, sizeof(unsigned int));
	memcpy(&this->langsCount, data + 29, sizeof(unsigned int));
	//Read lang codes
	unsigned int offset = 33;
	for (unsigned int lang_i = 0; lang_i < langsCount; lang_i++) {
		memcpy(&lang_codes[lang_i], data + offset, sizeof(unsigned int));
		offset += sizeof(unsigned int);
	}
	//Write all strings
	for (unsigned int str_i = 0; str_i < StringsCount; str_i++) {
		LocString str;
		//Read string ID
		while (data[offset] != '\0') {
			str.str_ID.push_back(data[offset]);
			offset += 1;
		}
		offset += 1;
		//Read integer Id
		memcpy(&str.ID, data + offset, sizeof(unsigned int));
		offset += sizeof(unsigned int);
		//Read string in all languages
		for (unsigned int lang_i = 0; lang_i < langsCount; lang_i++) {
			unsigned int lang_code = 0;
			memcpy(&lang_code, data + offset, sizeof(unsigned int));
			offset += sizeof(unsigned int);
			//Read string size
			unsigned int str_size = 0;
			memcpy(&str_size, data + offset, sizeof(unsigned int));
			offset += sizeof(unsigned int);
			//Read localized string
			for (unsigned int c_i = 0; c_i < str_size; c_i ++) {
				char32_t _char = 0;
				memcpy(&_char, data + offset, sizeof(char32_t));
				offset += sizeof(char32_t);
				str.STR[lang_code].push_back(_char);
			}
		}
		strings.push_back(str);
	}
	return true;
}
LocString* LocaleStringsFile::getString(unsigned int i) {
	return &strings[i];
}
LocString* LocaleStringsFile::getString(std::string str_ID) {
	for (unsigned int str_i = 0; str_i < StringsCount; str_i++) {
		LocString* ls = &strings[str_i];
		if (ls->str_ID.compare(str_ID) == 0)
			return ls;
	}
	return nullptr;
}
LocString* LocaleStringsFile::getStringById(unsigned int id) {
	for (unsigned int str_i = 0; str_i < StringsCount; str_i++) {
		LocString* ls = &strings[str_i];
		if (ls->ID == id)
			return ls;
	}
	return nullptr;
}
void LocaleStringsFile::pushString(LocString str) {
	strings.push_back(str);
	StringsCount += 1;
}
unsigned int LocaleStringsFile::getLang(unsigned int i) {
	return lang_codes[i];
}
unsigned int LocaleStringsFile::getLanguagesCount() {
	return this->langsCount;
}
unsigned int LocaleStringsFile::getStringsCount() {
	return this->StringsCount;
}
void LocaleStringsFile::pushLanguageCode(unsigned int lang_code) {
	lang_codes[langsCount] = lang_code;
	langsCount++;
}

LocString::LocString() {
	ID = 0;
}
LocString::~LocString() {

}