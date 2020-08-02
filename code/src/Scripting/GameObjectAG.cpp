#include "../../headers/Scripting/AngelScriptMgr.h"
#include "../../headers/world/World.h"

Engine::GameObject* newobj() {
	return new Engine::GameObject();
}

ZSVECTOR3* newvec() {
	return new ZSVECTOR3();
}

void Engine::bindGameObjectSDK(AGScriptMgr* mgr) {
	int result = 0;
	result = mgr->RegisterObjectType("GameObject", 0, asOBJ_REF | asOBJ_NOCOUNT);
	assert(result >= 0);
	result = mgr->RegisterObjectBehaviour("GameObject", asBEHAVE_FACTORY, "GameObject@ new_obj()", asFUNCTION(newobj), asCALL_CDECL);
	assert(result >= 0);
	result = mgr->RegisterObjectMethod("GameObject", "void setActive(bool)", asMETHOD(Engine::GameObject, setActive), asCALL_THISCALL);
	assert(result >= 0);
	mgr->RegisterObjectProperty("GameObject", "int propsNum", offsetof(Engine::GameObject, props_num));
	assert(result >= 0);

	

	result = mgr->RegisterObjectType("GameObjectProperty", 0, asOBJ_REF | asOBJ_NOCOUNT);
	assert(result >= 0);	
}

void Engine::bindGameObjectPropertiesSDK(AGScriptMgr* mgr) {
	int result = 0;

	result = mgr->RegisterObjectMethod("GameObject", "TransformProperty@ transform()", asMETHOD(Engine::GameObject, getPropertyPtr<TransformProperty>), asCALL_THISCALL);
	assert(result >= 0);
}

void Engine::bindMathSDK(AGScriptMgr* mgr) {
	int result = 0;
	result = mgr->RegisterObjectType("Vec3", sizeof(ZSVECTOR3), asOBJ_REF | asOBJ_NOCOUNT);
	assert(result >= 0);

	result = mgr->RegisterObjectBehaviour("Vec3", asBEHAVE_FACTORY, "Vec3@ new_vec()", asFUNCTION(newvec), asCALL_CDECL);
	assert(result >= 0);

	mgr->RegisterObjectProperty("Vec3", "float x", offsetof(ZSVECTOR3, X));
	mgr->RegisterObjectProperty("Vec3", "float y", offsetof(ZSVECTOR3, Y));
	mgr->RegisterObjectProperty("Vec3", "float z", offsetof(ZSVECTOR3, Z));
	mgr->RegisterObjectMethod("Vec3", " void Normalize()", asMETHOD(ZSVECTOR3, Normalize), asCALL_THISCALL);
	mgr->RegisterObjectMethod("Vec3", "Vec3@ opAddAssign(Vec3@)", asMETHOD(ZSVECTOR3, operator+=), asCALL_THISCALL);
}