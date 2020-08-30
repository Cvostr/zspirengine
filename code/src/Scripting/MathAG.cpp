#include "../../headers/Scripting/AngelScriptMgr.h"
#include "../../headers/world/World.h"
#include <cmath>

float ctg(float a) {
	return 1 / tan(a);
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

void Engine::bindMathSDK(AGScriptMgr* mgr) {
	mgr->RegisterObjectType(VEC3_TYPE_NAME, sizeof(ZSVECTOR3), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<ZSVECTOR3>());
	mgr->RegisterObjectBehaviour(VEC3_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(new_as_obj_T<ZSVECTOR3>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(VEC3_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(CreateVec3), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(VEC3_TYPE_NAME, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(del_as_obj_T<ZSVECTOR3>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectProperty(VEC3_TYPE_NAME, "float x", offsetof(ZSVECTOR3, X));
	mgr->RegisterObjectProperty(VEC3_TYPE_NAME, "float y", offsetof(ZSVECTOR3, Y));
	mgr->RegisterObjectProperty(VEC3_TYPE_NAME, "float z", offsetof(ZSVECTOR3, Z));
	mgr->RegisterObjectMethod(VEC3_TYPE_NAME, "void Normalize()", asMETHOD(ZSVECTOR3, Normalize), asCALL_THISCALL);
	mgr->RegisterObjectMethod(VEC3_TYPE_NAME, "Vec3 &opAddAssign(Vec3 &in)", asMETHOD(ZSVECTOR3, operator+=), asCALL_THISCALL);
	mgr->RegisterObjectMethod(VEC3_TYPE_NAME, "Vec3 &opAssign(Vec3 &in)", asMETHOD(ZSVECTOR3, operator=), asCALL_THISCALL);
	mgr->RegisterObjectMethod(VEC3_TYPE_NAME, "Vec3 opMul(float) const", asMETHOD(ZSVECTOR3, operator*), asCALL_THISCALL);
	mgr->RegisterObjectMethod(VEC3_TYPE_NAME, "Vec3 opAdd(const Vec3&in) const", asMETHOD(ZSVECTOR3, operator+), asCALL_THISCALL);
	mgr->RegisterObjectMethod(VEC3_TYPE_NAME, "bool opEquals(const Vec3&in) const", asMETHOD(ZSVECTOR3, operator ==), asCALL_THISCALL);
	//Bind Quaternion
	mgr->RegisterObjectType(QUAT_TYPE_NAME, sizeof(ZSQUATERNION), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<ZSQUATERNION>());
	mgr->RegisterObjectBehaviour(QUAT_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(new_as_obj_T<ZSQUATERNION>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(QUAT_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f(float, float, float, float)", asFUNCTION(CreateQuat), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(QUAT_TYPE_NAME, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(del_as_obj_T<ZSQUATERNION>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectProperty(QUAT_TYPE_NAME, "float x", offsetof(ZSQUATERNION, X));
	mgr->RegisterObjectProperty(QUAT_TYPE_NAME, "float y", offsetof(ZSQUATERNION, Y));
	mgr->RegisterObjectProperty(QUAT_TYPE_NAME, "float z", offsetof(ZSQUATERNION, Z));
	mgr->RegisterObjectProperty(QUAT_TYPE_NAME, "float w", offsetof(ZSQUATERNION, W));
	mgr->RegisterObjectMethod(QUAT_TYPE_NAME, "void Normalize()", asMETHOD(ZSQUATERNION, Normalize), asCALL_THISCALL);
	mgr->RegisterObjectMethod(QUAT_TYPE_NAME, "Quat &opAssign(Quat &in)", asMETHODPR(ZSQUATERNION, operator =, (ZSQUATERNION), ZSQUATERNION&), asCALL_THISCALL);

	//Bind RGB color
	mgr->RegisterObjectType(RGBCOLOR_TYPE_NAME, sizeof(ZSRGBCOLOR), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<ZSRGBCOLOR>());
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
	mgr->RegisterObjectMethod(CAM_TYPE_NAME, "void updateViewMat()", asMETHOD(Engine::Camera, updateViewMat), asCALL_THISCALL);
	mgr->RegisterObjectMethod(CAM_TYPE_NAME, "void updateProjectionMat()", asMETHOD(Engine::Camera, updateProjectionMat), asCALL_THISCALL);
	//Base Math methods
	mgr->RegisterGlobalFunction("float sin(float)", asFUNCTION(sinf), asCALL_CDECL);
	mgr->RegisterGlobalFunction("float cos(float)", asFUNCTION(cosf), asCALL_CDECL);
	mgr->RegisterGlobalFunction("float tan(float)", asFUNCTION(tanf), asCALL_CDECL);
	mgr->RegisterGlobalFunction("float ctg(float)", asFUNCTION(ctg), asCALL_CDECL);
	mgr->RegisterGlobalFunction("float log(float)", asFUNCTION(logf), asCALL_CDECL);
	mgr->RegisterGlobalFunction("float ln(float)", asFUNCTION(log10f), asCALL_CDECL);
	mgr->RegisterGlobalFunction("float rad(float)", asFUNCTION(DegToRad), asCALL_CDECL);
	mgr->RegisterGlobalFunction("Vec3 cross(const Vec3&in, const Vec3&in)", asFUNCTION(vCross), asCALL_CDECL);
	mgr->RegisterGlobalFunction("float dot(const Vec3&in, const Vec3&in)", asFUNCTION(vDot), asCALL_CDECL);
}
