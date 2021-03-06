#include "../../headers/Scripting/AngelScriptMgr.h"
#include "../../headers/world/World.hpp"
#include <cmath>

float ctg(float a) {
	return 1 / tan(a);
}

static void CreateVec3(float x, float y, float z, Vec3* ptr) {
	new (ptr) Vec3(x, y, z);
}

static void CreateQuat(float x, float y, float z, float w, Vec3* ptr) {
	new (ptr) Quaternion(x, y, z, w);
}

static void CreateRgbColor(int r, int g, int b, RGBAColor* ptr) {
	new (ptr) RGBAColor(r, g, b);
}

static void CreatePlane(const Vec3& Normal, float D, Engine::Plane* ptr) {
	new (ptr) Engine::Plane(Normal, D);
}

static void CreatePlaneI(const Vec3& Normal, const Vec3& Point, Engine::Plane* ptr) {
	new (ptr) Engine::Plane(Normal, Point);
}

void Engine::bindMathSDK(AGScriptMgr* mgr) {
	mgr->RegisterObjectType(VEC3_TYPE_NAME, sizeof(Vec3), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<Vec3>());
	mgr->RegisterObjectBehaviour(VEC3_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(new_as_obj_T<Vec3>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(VEC3_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(CreateVec3), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(VEC3_TYPE_NAME, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(del_as_obj_T<Vec3>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectProperty(VEC3_TYPE_NAME, "float x", offsetof(Vec3, X));
	mgr->RegisterObjectProperty(VEC3_TYPE_NAME, "float y", offsetof(Vec3, Y));
	mgr->RegisterObjectProperty(VEC3_TYPE_NAME, "float z", offsetof(Vec3, Z));
	mgr->RegisterObjectMethod(VEC3_TYPE_NAME, "void Normalize()", asMETHOD(Vec3, Normalize), asCALL_THISCALL);
	mgr->RegisterObjectMethod(VEC3_TYPE_NAME, "Vec3 &opAddAssign(Vec3 &in)", asMETHOD(Vec3, operator+=), asCALL_THISCALL);
	mgr->RegisterObjectMethod(VEC3_TYPE_NAME, "Vec3 &opAssign(Vec3 &in)", asMETHOD(Vec3, operator=), asCALL_THISCALL);
	mgr->RegisterObjectMethod(VEC3_TYPE_NAME, "Vec3 opMul(float) const", asMETHOD(Vec3, operator*), asCALL_THISCALL);
	mgr->RegisterObjectMethod(VEC3_TYPE_NAME, "Vec3 opAdd(const Vec3&in) const", asMETHOD(Vec3, operator+), asCALL_THISCALL);
	mgr->RegisterObjectMethod(VEC3_TYPE_NAME, "bool opEquals(const Vec3&in) const", asMETHOD(Vec3, operator ==), asCALL_THISCALL);
	//Bind Quaternion
	mgr->RegisterObjectType(QUAT_TYPE_NAME, sizeof(Quaternion), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<Quaternion>());
	mgr->RegisterObjectBehaviour(QUAT_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(new_as_obj_T<Quaternion>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(QUAT_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f(float, float, float, float)", asFUNCTION(CreateQuat), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(QUAT_TYPE_NAME, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(del_as_obj_T<Quaternion>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectProperty(QUAT_TYPE_NAME, "float x", offsetof(Quaternion, X));
	mgr->RegisterObjectProperty(QUAT_TYPE_NAME, "float y", offsetof(Quaternion, Y));
	mgr->RegisterObjectProperty(QUAT_TYPE_NAME, "float z", offsetof(Quaternion, Z));
	mgr->RegisterObjectProperty(QUAT_TYPE_NAME, "float w", offsetof(Quaternion, W));
	mgr->RegisterObjectMethod(QUAT_TYPE_NAME, "void Normalize()", asMETHOD(Quaternion, Normalize), asCALL_THISCALL);
	mgr->RegisterObjectMethod(QUAT_TYPE_NAME, "Quat &opAssign(Quat &in)", asMETHODPR(Quaternion, operator =, (Quaternion), Quaternion&), asCALL_THISCALL);
	//Bind RGB color
	mgr->RegisterObjectType(RGBACOLOR_TYPE_NAME, sizeof(RGBAColor), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<RGBAColor>());
	mgr->RegisterObjectBehaviour(RGBACOLOR_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(new_as_obj_T<RGBAColor>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(RGBACOLOR_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f(int, int, int)", asFUNCTION(CreateRgbColor), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(RGBACOLOR_TYPE_NAME, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(del_as_obj_T<RGBAColor>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectMethod(RGBACOLOR_TYPE_NAME, "RGBAColor &opAssign(RGBAColor &in)", asMETHODPR(RGBAColor, operator =, (RGBAColor), RGBAColor&), asCALL_THISCALL);
	mgr->RegisterObjectProperty(RGBACOLOR_TYPE_NAME, "int r", offsetof(RGBAColor, r));
	mgr->RegisterObjectProperty(RGBACOLOR_TYPE_NAME, "int g", offsetof(RGBAColor, g));
	mgr->RegisterObjectProperty(RGBACOLOR_TYPE_NAME, "int b", offsetof(RGBAColor, b));
	//Bind Plane
	mgr->RegisterObjectType(PLANE_TYPE_NAME, sizeof(Plane), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<Plane>());
	mgr->RegisterObjectBehaviour(PLANE_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(new_as_obj_T<Plane>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(PLANE_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f(const Vec3& in, float)", asFUNCTION(CreatePlane), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(PLANE_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f(const Vec3& in, const Vec3 &in)", asFUNCTION(CreatePlaneI), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(PLANE_TYPE_NAME, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(del_as_obj_T<Plane>), asCALL_CDECL_OBJLAST);
	
	//Base Math methods
	mgr->RegisterGlobalFunction("float sqrt(float)", asFUNCTION(sqrtf), asCALL_CDECL);
	mgr->RegisterGlobalFunction("float sin(float)", asFUNCTION(sinf), asCALL_CDECL);
	mgr->RegisterGlobalFunction("float cos(float)", asFUNCTION(cosf), asCALL_CDECL);
	mgr->RegisterGlobalFunction("float tan(float)", asFUNCTION(tanf), asCALL_CDECL);
	mgr->RegisterGlobalFunction("float ctg(float)", asFUNCTION(ctg), asCALL_CDECL);
	mgr->RegisterGlobalFunction("float log(float)", asFUNCTION(logf), asCALL_CDECL);
	mgr->RegisterGlobalFunction("float ln(float)", asFUNCTION(log10f), asCALL_CDECL);
	mgr->RegisterGlobalFunction("float abs(float)", asFUNCTION(fabsf), asCALL_CDECL);
	mgr->RegisterGlobalFunction("float rad(float)", asFUNCTION(DegToRad), asCALL_CDECL);
	mgr->RegisterGlobalFunction("Vec3 cross(const Vec3&in, const Vec3&in)", asFUNCTION(vCross), asCALL_CDECL);
	mgr->RegisterGlobalFunction("float dot(const Vec3&in, const Vec3&in)", asFUNCTION(vDot), asCALL_CDECL);
}
