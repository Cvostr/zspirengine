#include "../../headers/Scripting/FilesAG.hpp"
#include "../../headers/Scripting/AngelScriptMgr.h"

Engine::KV_RECORD* Engine::KeyValueFile::findRecordByKey(std::string key) {
	unsigned int recordsNum = records.size();

	for (unsigned int r_i = 0; r_i < recordsNum; r_i++) {
		KV_RECORD* record = records[r_i];

		if (record->key.compare(key) == 0)
			return record;
	}

	return nullptr;
}

void Engine::bindFilesSDK(AGScriptMgr* mgr) {
	mgr->RegisterObjectType(KV_FILE_OBJ, sizeof(Engine::KeyValueFile), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<Engine::KeyValueFile>());
}