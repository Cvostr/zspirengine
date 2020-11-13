#pragma once

#include "Vec3.hpp"

class ZSVECTOR4 {
public:
	float X;
	float Y;
	float Z;
	float W;

	ZSVECTOR3 _Vec3() {
		return ZSVECTOR3(X, Y, Z);
	}

	ZSVECTOR4() : X(0), Y(0), Z(0), W(0) {}

	ZSVECTOR4(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) {}

	ZSVECTOR4(const ZSVECTOR3& v, float w) {
		this->X = v.X;
		this->Y = v.Y;
		this->Z = v.Z;
		this->W = w;
	}

	ZSVECTOR4(float v) : X(v), Y(v), Z(v), W(v) {}

};