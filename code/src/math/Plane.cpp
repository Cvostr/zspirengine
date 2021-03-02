#include "../../headers/math/Plane.hpp"

void Engine::Plane::Create(const Vec3& Normal, const Vec3& Point) {
	mNormal = Normal;
	mNormal.Normalize();

	mD = vDot((mNormal * -1), Point);
}
void Engine::Plane::Create(const Vec3& v1, const Vec3& v2, const Vec3& v3) {
	Vec3 v1v2 = v2 - v1;
	Vec3 v1v3 = v3 - v1;

	Vec3 Normal = vCross(v1, v2);
	Create(Normal, v3);
}

Mat4 Engine::Plane::GetReflectionMatrix() {
	return GetPlaneReflectionMat(mNormal.X, mNormal.Y, mNormal.Z, mD);
}