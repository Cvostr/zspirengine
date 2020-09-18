#include "../../headers/misc/KeyValueFile.hpp"
#include <fstream>

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
	std::ofstream stream(file_path, std::ofstream::binary);


	stream.close();
}

Engine::KeyValueFile::KeyValueFile() {

}
Engine::KeyValueFile::~KeyValueFile() {

}