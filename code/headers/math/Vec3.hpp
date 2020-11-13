#pragma once

#include "math.h"
#include "Quat.hpp"
#define REAL_NUM_EQ(x, y) (fabs((x) - (y)) < 0.000001f)

class ZSVECTOR3 {
public:
	float X;
	float Y;
	float Z;

	void Normalize() {
		float sqr = X * X + Y * Y + Z * Z;
		if (REAL_NUM_EQ(sqr, 1) || REAL_NUM_EQ(sqr, 0))
			return;

		float invrt = (1.0f / static_cast<float>(sqrt(sqr)));
		X *= invrt;
		Y *= invrt;
		Z *= invrt;
	}

	float Length() { //Calculates module length of vector
		return sqrtf(X * X + Y * Y + Z * Z);
	}

	ZSVECTOR3& operator+=(const ZSVECTOR3& r) {
		X += r.X;
		Y += r.Y;
		Z += r.Z;
		return *this;
	}
	ZSVECTOR3& operator*=(const float& d) {
		X *= d;
		Y *= d;
		Z *= d;
		return *this;
	}

	ZSVECTOR3& operator=(ZSVECTOR3 r) {
		X = r.X;
		Y = r.Y;
		Z = r.Z;
		return *this;
	}

	ZSVECTOR3 operator*(float d) const {
		return ZSVECTOR3(X * d, Y * d, Z * d);
	}

	ZSVECTOR3 operator+(const ZSVECTOR3& r) const {
		return ZSVECTOR3(X + r.X, Y + r.Y, Z + r.Z);
	}

	bool operator ==(const ZSVECTOR3& r) const {
		return X == r.X && Y == r.Y && Z == r.Z;
	}

	ZSVECTOR3() : X(0), Y(0), Z(0) {}

	ZSVECTOR3(float nx, float ny, float nz) : X(nx), Y(ny), Z(nz) {}

	ZSVECTOR3(float v) : X(v), Y(v), Z(v) {}

};

inline ZSVECTOR3 operator-(const ZSVECTOR3& l, const ZSVECTOR3& r)
{
	ZSVECTOR3 Ret(l.X - r.X, l.Y - r.Y, l.Z - r.Z);

	return Ret;
}

inline ZSVECTOR3 operator/(const ZSVECTOR3& l, const float& d)
{
	ZSVECTOR3 Ret(l.X / d, l.Y / d, l.Z / d);

	return Ret;
}
inline ZSVECTOR3 operator*(const ZSVECTOR3& l, const ZSVECTOR3& r)
{
	ZSVECTOR3 Ret(l.X * r.X, l.Y * r.Y, l.Z * r.Z);

	return Ret;
}
inline ZSVECTOR3 operator/(const ZSVECTOR3& l, const ZSVECTOR3& r)
{
	ZSVECTOR3 Ret(l.X / r.X, l.Y / r.Y, l.Z / r.Z);

	return Ret;
}
inline bool operator==(const ZSVECTOR3& l, const ZSVECTOR3& r)
{
	bool result = false;
	if (REAL_NUM_EQ(l.X, r.X) && REAL_NUM_EQ(l.Y, r.Y) && REAL_NUM_EQ(l.Z, r.Z)) result = true;

	return result;
}
inline bool operator!=(const ZSVECTOR3& l, const ZSVECTOR3& r)
{
	bool result = false;
	if (!REAL_NUM_EQ(l.X, r.X) || !REAL_NUM_EQ(l.Y, r.Y) || !REAL_NUM_EQ(l.Z, r.Z)) result = true;

	return result;
}

ZSVECTOR3 vCross(const ZSVECTOR3& v1, const ZSVECTOR3& v2);
ZSVECTOR3 vmul(ZSVECTOR3& v, float f);
ZSVECTOR3 vadd(ZSVECTOR3& v1, ZSVECTOR3& v2);
ZSVECTOR3 quatToEuler(const ZSQUATERNION& q);
ZSVECTOR3 lerp(ZSVECTOR3 v1, ZSVECTOR3 v2, float factor);
float getDistance(const ZSVECTOR3& p1, const ZSVECTOR3& p2);
ZSVECTOR3 _getDirection(float pitch, float yaw, float roll);
float vDot(const ZSVECTOR3& v1, const ZSVECTOR3& v2);
bool isDistanceFits(const ZSVECTOR3& pos1, const ZSVECTOR3& pos2, float max_dist);