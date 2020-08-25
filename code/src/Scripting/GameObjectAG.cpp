#include "../../headers/Scripting/AngelScriptMgr.h"
#include "../../headers/world/World.h"
#include "../../headers/world/go_properties.h"


Engine::GameObject* newobj() {
	return new Engine::GameObject();
}	

static void CreateVec3(float x, float y, float z, ZSVECTOR3* ptr) {
	new (ptr) ZSVECTOR3(x, y, z);
}

static void CreateQuat(float x, float y, float z, float w, ZSVECTOR3* ptr) {
	new (ptr) ZSQUATERNION(x, y, z, w);
}

static void CreateRgbColor(int r, int g, int b, ZSRGBCOLOR* ptr) {
	new (ptr) ZSRGBCOLOR(r, g, b);
}

void Engine::bindGameObjectSDK(AGScriptMgr* mgr) {
	//Regiter base GameObject
	mgr->RegisterObjectType(GAME_OBJECT_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);
	mgr->RegisterObjectBehaviour(GAME_OBJECT_TYPE_NAME, asBEHAVE_FACTORY, "GameObject@ new_obj()", asFUNCTION(new_as_ref_T<Engine::GameObject>), asCALL_CDECL);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, "void setActive(bool)", asMETHOD(Engine::GameObject, setActive), asCALL_THISCALL);
	mgr->RegisterObjectProperty(GAME_OBJECT_TYPE_NAME, "int propsNum", offsetof(Engine::GameObject, props_num));

	//Register GameObjectProperty class
	mgr->RegisterObjectType(GAME_OBJECT_PROP_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);	
}

void Engine::bindWorldSDK(AGScriptMgr* mgr) {
	mgr->RegisterObjectType(WORLD_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);
	mgr->RegisterObjectMethod(WORLD_TYPE_NAME, "GameObject@ findObject(string)", asMETHOD(Engine::World, getGameObjectByLabel), asCALL_THISCALL);
	mgr->RegisterObjectMethod(WORLD_TYPE_NAME, "void removeObject(GameObject@)", asMETHOD(Engine::World, removeObject), asCALL_THISCALL);
	mgr->RegisterObjectProperty(WORLD_TYPE_NAME, "Camera@ camera", offsetof(Engine::World, world_camera));

	mgr->RegisterObjectProperty(GAME_OBJECT_TYPE_NAME, "World@ world", offsetof(Engine::GameObject, world_ptr));
}

void Engine::bindGameObjectPropertiesSDK(AGScriptMgr* mgr) {
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, "Transform@ transform()", asMETHOD(Engine::GameObject, getPropertyPtr<TransformProperty>), asCALL_THISCALL);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, "LightSource@ light()", asMETHOD(Engine::GameObject, getPropertyPtr<LightsourceProperty>), asCALL_THISCALL);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, "AudioSource@ audio()", asMETHOD(Engine::GameObject, getPropertyPtr<AudioSourceProperty>), asCALL_THISCALL);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, "CharacterController@ char_ctrl()", asMETHOD(Engine::GameObject, getPropertyPtr<CharacterControllerProperty>), asCALL_THISCALL);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, "Animation@ anim()", asMETHOD(Engine::GameObject, getPropertyPtr<AnimationProperty>), asCALL_THISCALL);
}

void Engine::bindMathSDK(AGScriptMgr* mgr) {
	mgr->RegisterObjectType(VEC3_TYPE_NAME, sizeof(ZSVECTOR3), asOBJ_VALUE);
	mgr->RegisterObjectBehaviour(VEC3_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(new_as_obj_T<ZSVECTOR3>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(VEC3_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(CreateVec3), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(VEC3_TYPE_NAME, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(del_as_obj_T<ZSVECTOR3>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectProperty(VEC3_TYPE_NAME, "float x", offsetof(ZSVECTOR3, X));
	mgr->RegisterObjectProperty(VEC3_TYPE_NAME, "float y", offsetof(ZSVECTOR3, Y));
	mgr->RegisterObjectProperty(VEC3_TYPE_NAME, "float z", offsetof(ZSVECTOR3, Z));
	mgr->RegisterObjectMethod(VEC3_TYPE_NAME, "void Normalize()", asMETHOD(ZSVECTOR3, Normalize), asCALL_THISCALL);
	mgr->RegisterObjectMethod(VEC3_TYPE_NAME, "Vec3 &opAddAssign(Vec3 &in)", asMETHOD(ZSVECTOR3, operator+=), asCALL_THISCALL);
	mgr->RegisterObjectMethod(VEC3_TYPE_NAME, "Vec3 &opAssign(Vec3 &in)", asMETHODPR(ZSVECTOR3, operator =, (ZSVECTOR3), ZSVECTOR3&), asCALL_THISCALL);
	//Bind Quaternion
	mgr->RegisterObjectType(QUAT_TYPE_NAME, sizeof(ZSQUATERNION), asOBJ_VALUE);
	mgr->RegisterObjectBehaviour(QUAT_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(new_as_obj_T<ZSQUATERNION>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(QUAT_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f(float, float, float, float)", asFUNCTION(CreateQuat), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(QUAT_TYPE_NAME, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(del_as_obj_T<ZSQUATERNION>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectProperty(QUAT_TYPE_NAME, "float x", offsetof(ZSQUATERNION, X));
	mgr->RegisterObjectProperty(QUAT_TYPE_NAME, "float y", offsetof(ZSQUATERNION, Y));
	mgr->RegisterObjectProperty(QUAT_TYPE_NAME, "float z", offsetof(ZSQUATERNION, Z));
	mgr->RegisterObjectProperty(QUAT_TYPE_NAME, "float w", offsetof(ZSQUATERNION, W));
	//mgr->RegisterObjectMethod(QUAT_TYPE_NAME, "void Normalize()", asMETHOD(ZSQUATERNION, Normalize), asCALL_THISCALL);
	mgr->RegisterObjectMethod(QUAT_TYPE_NAME, "Quat &opAssign(Quat &in)", asMETHODPR(ZSQUATERNION, operator =, (ZSQUATERNION), ZSQUATERNION&), asCALL_THISCALL);

	//Bind RGB color
	mgr->RegisterObjectType(RGBCOLOR_TYPE_NAME, sizeof(ZSRGBCOLOR), asOBJ_VALUE);
	mgr->RegisterObjectBehaviour(RGBCOLOR_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(new_as_obj_T<ZSRGBCOLOR>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(RGBCOLOR_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f(int, int, int)", asFUNCTION(CreateRgbColor), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(RGBCOLOR_TYPE_NAME, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(del_as_obj_T<ZSRGBCOLOR>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectMethod(RGBCOLOR_TYPE_NAME, "rgbColor &opAssign(rgbColor &in)", asMETHODPR(ZSRGBCOLOR, operator =, (ZSRGBCOLOR), ZSRGBCOLOR&), asCALL_THISCALL);
	mgr->RegisterObjectProperty(RGBCOLOR_TYPE_NAME, "int r", offsetof(ZSRGBCOLOR, r));
	mgr->RegisterObjectProperty(RGBCOLOR_TYPE_NAME, "int g", offsetof(ZSRGBCOLOR, g));
	mgr->RegisterObjectProperty(RGBCOLOR_TYPE_NAME, "int b", offsetof(ZSRGBCOLOR, b));
	//Bind Camera
	mgr->RegisterObjectType(CAM_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);
	mgr->RegisterObjectProperty(CAM_TYPE_NAME, "Vec3 pos", offsetof(Engine::Camera, camera_pos));
	mgr->RegisterObjectProperty(CAM_TYPE_NAME, "Vec3 front", offsetof(Engine::Camera, camera_front));
	mgr->RegisterObjectProperty(CAM_TYPE_NAME, "Vec3 up", offsetof(Engine::Camera, camera_up));
	mgr->RegisterObjectProperty(CAM_TYPE_NAME, "float FOV", offsetof(Engine::Camera, FOV));
	mgr->RegisterObjectProperty(CAM_TYPE_NAME, "float nearZ", offsetof(Engine::Camera, nearZ));
	mgr->RegisterObjectProperty(CAM_TYPE_NAME, "float farZ", offsetof(Engine::Camera, farZ));
}

void Engine::bindResourceSDK(const char* className, AGScriptMgr* mgr) {
	mgr->RegisterObjectType(className, 0, asOBJ_REF | asOBJ_NOCOUNT);

	mgr->RegisterObjectProperty(className, "string label", offsetof(ZsResource, rel_path));
	mgr->RegisterObjectProperty(className, "string path", offsetof(ZsResource, resource_label));
}



void Engine::bindResourcesSDK(AGScriptMgr* mgr) {
	bindResourceSDK(TEXRESOURCE_TYPE_NAME, mgr);
	bindResourceSDK(MESHRESOURCE_TYPE_NAME, mgr);
	bindResourceSDK(AUDRESOURCE_TYPE_NAME, mgr);
}