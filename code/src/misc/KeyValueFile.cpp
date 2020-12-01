#include "../../headers/misc/KeyValueFile.hpp"
#include <fstream>
#include <type_traits>
#include "../../headers/misc/misc.h"

#define CMP_TYPES(a, b)  a->_typeid == typeid(b).hash_code()
char zero = '\0';

Engine::KV_RECORD* Engine::KeyValueFile::findRecordByKey(std::string key) {
	unsigned int recordsNum = records.size();

	for (unsigned int r_i = 0; r_i < recordsNum; r_i++) {
		KV_RECORD* record = records[r_i];

		if (record->key.compare(key) == 0)
			return record;
	}

	return nullptr;
}

void Engine::KeyValueFile::ReadFile(std::string path) {
	std::ifstream stream;
	stream.open(path, std::ifstream::binary);
	this->file_path = path;

	if (!stream.is_open()) {

	}

	stream.close();
}

void Engine::KeyValueFile::saveAll() {
	ZsStream stream;
	stream.open(file_path, std::ofstream::binary);

	unsigned int recordsNum = records.size();

	for (unsigned int r_i = 0; r_i < recordsNum; r_i++) {
		KV_RECORD* record = records[r_i];
		//write record name
		stream.write(record->key.c_str(), record->key.size());
		stream.write(&zero, 1);

		if (CMP_TYPES(record, int)) {
			int value = GetValue<int>(record);
			stream.writeBinaryValue(&value);
		}
		if (CMP_TYPES(record, float)) {
			float value = GetValue<float>(record);
			stream.writeBinaryValue(&value);
		}
		if (CMP_TYPES(record, double)) {
			double value = GetValue<double>(record);
			stream.writeBinaryValue(&value);
		}
		if (CMP_TYPES(record, bool)) {
			bool value = GetValue<bool>(record);
			stream.writeBinaryValue(&value);
		}
		if (CMP_TYPES(record, std::string)) {
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