#include "../../headers/misc/KeyValueFile.hpp"
#include <fstream>
#include <type_traits>
#include "../../headers/misc/misc.h"

#define CMP_TYPES(a, b)  a->_typeid == typeid(b).hash_code()
char zero = '\0';

char v_int = 1;
char v_float = 2;
char v_double = 3;
char v_bool = 4;
char v_string = 5;

Engine::KV_RECORD* Engine::KeyValueFile::findRecordByKey(std::string key) {
	size_t recordsNum = records.size();

	for (size_t r_i = 0; r_i < recordsNum; r_i++) {
		KV_RECORD* record = records[r_i];

		if (record->key.compare(key) == 0)
			return record;
	}

	return nullptr;
}

void Engine::KeyValueFile::ReadFile(std::string path) {
	this->records.clear();
	this->file_path = path;
	std::ifstream stream;
	stream.open(path, std::ifstream::binary | std::ifstream::ate);

	if (!stream.is_open()) {
		return;
	}

	size_t Size = stream.tellg();
	stream.seekg(0);

	char* Data = new char[Size];
	stream.read(Data, Size);
	
	size_t recordsNum = 0;
	unsigned int Offset = 0;
	readBinaryValue(&recordsNum, Data, Offset);

	size_t processedRecords = 0;

	std::string prefix;

	while (processedRecords < recordsNum) {
		prefix.clear();
		readString(prefix, Data, Offset);

		char Type = 0;
		readBinaryValue(&Type, Data + Offset, Offset);
		processedRecords++;

		if (Type == v_int) {
			KV<int>* vl = new KV<int>;
			vl->key = prefix;
			readBinaryValue(&vl->value, Data + Offset, Offset);
			records.push_back(vl);
		}
		if (Type == v_float) {
			KV<float>* vl = new KV<float>;
			vl->key = prefix;
			readBinaryValue(&vl->value, Data + Offset, Offset);
			records.push_back(vl);
		}
		if (Type == v_double) {
			KV<double>* vl = new KV<double>;
			vl->key = prefix;
			readBinaryValue(&vl->value, Data + Offset, Offset);
			records.push_back(vl);
		}
		if (Type == v_bool) {
			KV<bool>* vl = new KV<bool>;
			vl->key = prefix;
			readBinaryValue(&vl->value, Data + Offset, Offset);
			records.push_back(vl);
		}
		if (Type == v_string) {
			KV<std::string>* vl = new KV<std::string>;
			vl->key = prefix;
			readString(vl->value, Data, Offset);
			records.push_back(vl);
		}
	}


	stream.close();
}

void Engine::KeyValueFile::SaveAll() {
	ZsStream stream;
	stream.open(file_path, std::ofstream::binary);

	size_t recordsNum = records.size();
	stream.writeBinaryValue(&recordsNum);

	for (size_t r_i = 0; r_i < recordsNum; r_i++) {
		KV_RECORD* record = records[r_i];
		//write record name
		stream.write(record->key.c_str(), record->key.size());
		stream.write(&zero, 1);

		if (CMP_TYPES(record, int)) {
			stream.writeBinaryValue(&v_int);
			int value = GetValue<int>(record);
			stream.writeBinaryValue(&value);
		}
		if (CMP_TYPES(record, float)) {
			stream.writeBinaryValue(&v_float);
			float value = GetValue<float>(record);
			stream.writeBinaryValue(&value);
		}
		if (CMP_TYPES(record, double)) {
			stream.writeBinaryValue(&v_double);
			double value = GetValue<double>(record);
			stream.writeBinaryValue(&value);
		}
		if (CMP_TYPES(record, bool)) {
			stream.writeBinaryValue(&v_bool);
			bool value = GetValue<bool>(record);
			stream.writeBinaryValue(&value);
		}
		if (CMP_TYPES(record, std::string)) {
			stream.writeBinaryValue(&v_string);
			std::string value = GetValue<std::string>(record);
			stream.writeString(value);
		}
	}

	stream.close();
}

Engine::KeyValueFile::KeyValueFile() {

}
Engine::KeyValueFile::~KeyValueFile() {

}