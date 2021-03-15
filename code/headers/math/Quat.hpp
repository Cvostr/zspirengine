#pragma once

#include <cmath>
#define REAL_NUM_EQ(x, y) (fabs((x) - (y)) < 0.000001f)

class Quaternion {
public:
	float X;
	float Y;
	float Z;
	float W;

	Quaternion() {
		X = 0;
		Y = 0;
		Z = 0;
		W = 0;
	}

	Quaternion(float x, float y, float z, float w) {
		this->X = x;
		this->Y = y;
		this->Z = z;
		this->W = w;
	}

	void Normalize() {
		float m = X * X + Y * Y + Z * Z + W * W;

		if (REAL_NUM_EQ(m, 1) || REAL_NUM_EQ(m, 0))
			return;

		m = 1.f / static_cast<float>(sqrt(m));

		X *= m;
		Y *= m;
		Z *= m;
		W *= m;
	}

	Quaternion& operator=(Quaternion r) {
		X = r.X;
		Y = r.Y;
		Z = r.Z;
		W = r.W;
		return *this;
	}
};

Quaternion slerp(Quaternion q1, Quaternion q2, float factor);