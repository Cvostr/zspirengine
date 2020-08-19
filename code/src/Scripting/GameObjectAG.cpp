#include "../../headers/Scripting/AngelScriptMgr.h"
#include "../../headers/world/World.h"
#include "../../headers/world/go_properties.h"


Engine::GameObject* newobj() {
	return new Engine::GameObject();
}

ZSVECTOR3* newvec() {
	return new ZSVECTOR3();
}

ZSVECTOR3* newvec_xyz(float x, float y, float z) {
	return new ZSVECTOR3(x, y, z);
}

ZSRGBCOLOR* newcolor() {
	return new ZSRGBCOLOR();
}

void Engine::bindGameObjectSDK(AGScriptMgr* mgr) {
	int result = 0;
	//Regiter base GameObject
	result = mgr->RegisterObjectType(GAME_OBJECT_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);
	assert(result >= 0);
	result = mgr->RegisterObjectBehaviour(GAME_OBJECT_TYPE_NAME, asBEHAVE_FACTORY, "GameObject@ new_obj()", asFUNCTION(newobj), asCALL_CDECL);
	assert(result >= 0);
	result = mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, "void setActive(bool)", asMETHOD(Engine::GameObject, setActive), asCALL_THISCALL);
	assert(result >= 0);
	mgr->RegisterObjectProperty(GAME_OBJECT_TYPE_NAME, "int propsNum", offsetof(Engine::GameObject, props_num));
	assert(result >= 0);

	//Register GameObjectProperty class
	result = mgr->RegisterObjectType(GAME_OBJECT_PROP_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);
	assert(result >= 0);	
}

void Engine::bindGameObjectPropertiesSDK(AGScriptMgr* mgr) {
	int result = 0;

	result = mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, "Transform@ transform()", asMETHOD(Engine::GameObject, getPropertyPtr<TransformProperty>), asCALL_THISCALL);
	assert(result >= 0);
	result = mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, "LightSource@ light()", asMETHOD(Engine::GameObject, getPropertyPtr<LightsourceProperty>), asCALL_THISCALL);
	assert(result >= 0);
	result = mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, "AudioSource@ audio()", asMETHOD(Engine::GameObject, getPropertyPtr<AudioSourceProperty>), asCALL_THISCALL);
	assert(result >= 0);
}

void Engine::bindMathSDK(AGScriptMgr* mgr) {
	int result = 0;
	result = mgr->RegisterObjectType(VEC3_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);
	assert(result >= 0);
	result = mgr->RegisterObjectBehaviour(VEC3_TYPE_NAME, asBEHAVE_FACTORY, "Vec3@ Vec3()", asFUNCTION(newvec), asCALL_CDECL);
	assert(result >= 0);
	result = mgr->RegisterObjectBehaviour(VEC3_TYPE_NAME, asBEHAVE_FACTORY, "Vec3@ Vec3(float, float, float)", asFUNCTION(newvec_xyz), asCALL_CDECL);
	assert(result >= 0);
	mgr->RegisterObjectProperty(VEC3_TYPE_NAME, "float x", offsetof(ZSVECTOR3, X));
	mgr->RegisterObjectProperty(VEC3_TYPE_NAME, "float y", offsetof(ZSVECTOR3, Y));
	mgr->RegisterObjectProperty(VEC3_TYPE_NAME, "float z", offsetof(ZSVECTOR3, Z));
	mgr->RegisterObjectMethod(VEC3_TYPE_NAME, " void Normalize()", asMETHOD(ZSVECTOR3, Normalize), asCALL_THISCALL);
	mgr->RegisterObjectMethod(VEC3_TYPE_NAME, "Vec3@ opAddAssign(Vec3@)", asMETHOD(ZSVECTOR3, operator+=), asCALL_THISCALL);
	//Bind RGB color
	result = mgr->RegisterObjectType(RGBCOLOR_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);
	assert(result >= 0);
	result = mgr->RegisterObjectBehaviour(RGBCOLOR_TYPE_NAME, asBEHAVE_FACTORY, "rgbColor@ new_col()", asFUNCTION(newcolor), asCALL_CDECL);
	assert(result >= 0);
	mgr->RegisterObjectProperty(RGBCOLOR_TYPE_NAME, "int r", offsetof(ZSRGBCOLOR, r));
	mgr->RegisterObjectProperty(RGBCOLOR_TYPE_NAME, "int g", offsetof(ZSRGBCOLOR, g));
	mgr->RegisterObjectProperty(RGBCOLOR_TYPE_NAME, "int b", offsetof(ZSRGBCOLOR, b));
}