#include "../../headers/misc/KeyValueFile.hpp"
#include "../../headers/Scripting/AngelScriptMgr.h"

void Engine::bindFilesSDK(AGScriptMgr* mgr) {
	mgr->RegisterObjectType(KV_FILE_OBJ, sizeof(Engine::KeyValueFile), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<Engine::KeyValueFile>());
	mgr->RegisterObjectBehaviour(KV_FILE_OBJ, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(new_as_obj_T<Engine::KeyValueFile>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(KV_FILE_OBJ, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(del_as_obj_T<Engine::KeyValueFile>), asCALL_CDECL_OBJLAST);

	mgr->RegisterObjectMethod(KV_FILE_OBJ, "void setInt(string, int)", asMETHOD(Engine::KeyValueFile, setValue<int>), asCALL_THISCALL);
	mgr->RegisterObjectMethod(KV_FILE_OBJ, "void setFloat(string, float)", asMETHOD(Engine::KeyValueFile, setValue<float>), asCALL_THISCALL);
	mgr->RegisterObjectMethod(KV_FILE_OBJ, "void setBool(string, bool)", asMETHOD(Engine::KeyValueFile, setValue<bool>), asCALL_THISCALL);
	mgr->RegisterObjectMethod(KV_FILE_OBJ, "void setVec3(string, Vec3)", asMETHOD(Engine::KeyValueFile, setValue<ZSVECTOR3>), asCALL_THISCALL);
	mgr->RegisterObjectMethod(KV_FILE_OBJ, "void setString(string, string)", asMETHOD(Engine::KeyValueFile, setValue<std::string>), asCALL_THISCALL);

	mgr->RegisterObjectMethod(KV_FILE_OBJ, "void ReadFile(string)", asMETHOD(Engine::KeyValueFile, ReadFile), asCALL_THISCALL);
	mgr->RegisterObjectMethod(KV_FILE_OBJ, "void SaveAll()", asMETHOD(Engine::KeyValueFile, saveAll), asCALL_THISCALL);
}