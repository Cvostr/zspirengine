#include "../../headers/misc/KeyValueFile.hpp"
#include "../../headers/Scripting/AngelScriptMgr.h"

void Engine::bindFilesSDK(AGScriptMgr* mgr) {
	mgr->RegisterObjectType(KV_FILE_OBJ, 0, asOBJ_REF | asOBJ_NOCOUNT);
	mgr->RegisterObjectBehaviour(KV_FILE_OBJ, asBEHAVE_FACTORY, "KeyValueFile@ f()", asFUNCTION(new_as_ref_T<Engine::KeyValueFile>), asCALL_CDECL);

	mgr->RegisterObjectMethod(KV_FILE_OBJ, "void SetInt(string, int)", asFUNCTION(ASSetValue<int>), asCALL_CDECL_OBJFIRST);
	mgr->RegisterObjectMethod(KV_FILE_OBJ, "void SetFloat(string, float)", asFUNCTION(ASSetValue<float>), asCALL_CDECL_OBJFIRST);
	mgr->RegisterObjectMethod(KV_FILE_OBJ, "void SetDouble(string, double)", asFUNCTION(ASSetValue<double>), asCALL_CDECL_OBJFIRST);
	mgr->RegisterObjectMethod(KV_FILE_OBJ, "void SetBool(string, bool)", asFUNCTION(ASSetValue<bool>), asCALL_CDECL_OBJFIRST);
	mgr->RegisterObjectMethod(KV_FILE_OBJ, "void SetString(string, string)", asFUNCTION(ASSetValue<std::string>), asCALL_CDECL_OBJFIRST);
	mgr->RegisterObjectMethod(KV_FILE_OBJ, "void SetVec3(string, Vec3)", asFUNCTION(ASSetValue<Vec3>), asCALL_CDECL_OBJFIRST);
	
	mgr->RegisterObjectMethod(KV_FILE_OBJ, "int GetInt(string)", asFUNCTION(ASGetValue<int>), asCALL_CDECL_OBJFIRST);
	mgr->RegisterObjectMethod(KV_FILE_OBJ, "float GetFloat(string)", asFUNCTION(ASGetValue<float>), asCALL_CDECL_OBJFIRST);
	mgr->RegisterObjectMethod(KV_FILE_OBJ, "double GetDouble(string)", asFUNCTION(ASGetValue<double>), asCALL_CDECL_OBJFIRST);
	mgr->RegisterObjectMethod(KV_FILE_OBJ, "bool GetBool(string)", asFUNCTION(ASGetValue<bool>), asCALL_CDECL_OBJFIRST);
	mgr->RegisterObjectMethod(KV_FILE_OBJ, "string GetString(string)", asFUNCTION(ASGetValue<std::string>), asCALL_CDECL_OBJFIRST);
	mgr->RegisterObjectMethod(KV_FILE_OBJ, "Vec3 GetVec3(string)", asFUNCTION(ASGetValue<Vec3>), asCALL_CDECL_OBJFIRST);

	mgr->RegisterObjectMethod(KV_FILE_OBJ, "void ReadFile(string)", asMETHOD(Engine::KeyValueFile, ReadFile), asCALL_THISCALL);
	mgr->RegisterObjectMethod(KV_FILE_OBJ, "void SaveAll()", asMETHOD(Engine::KeyValueFile, SaveAll), asCALL_THISCALL);
}