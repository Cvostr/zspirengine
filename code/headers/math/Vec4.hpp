#pragma once

#include "Vec3.hpp"

class Vec4 {
public:
	float X;
	float Y;
	float Z;
	float W;

	Vec3 _Vec3() {
		return Vec3(X, Y, Z);
	}

	Vec4() : X(0), Y(0), Z(0), W(0) {}

	Vec4(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) {}

	Vec4(const Vec3& v, float w) {
		this->X = v.X;
		this->Y = v.Y;
		this->Z = v.Z;
		this->W = w;
	}

	Vec4(float v) : X(v), Y(v), Z(v), W(v) {}

};