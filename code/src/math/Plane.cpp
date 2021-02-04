#include "../../headers/math/Plane.hpp"

Engine::Plane::Plane(Vec3 Normal, Vec3 Point) {
	mNormal = Normal;
	mNormal.Normalize();

	mD = vDot((mNormal * -1), Point);
}

Mat4 Engine::Plane::GetReflectionMatrix() {
	return GetPlaneReflectionMat(mNormal.X, mNormal.Y, mNormal.Z, mD);
}