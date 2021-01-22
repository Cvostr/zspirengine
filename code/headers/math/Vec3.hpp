#pragma once

#include "math.h"
#include "Quat.hpp"
#define REAL_NUM_EQ(x, y) (fabs((x) - (y)) < 0.000001f)

class Vec3 {
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

	Vec3& operator+=(const Vec3& r) {
		X += r.X;
		Y += r.Y;
		Z += r.Z;
		return *this;
	}
	Vec3& operator*=(const float& d) {
		X *= d;
		Y *= d;
		Z *= d;
		return *this;
	}

	Vec3& operator=(Vec3 r) {
		X = r.X;
		Y = r.Y;
		Z = r.Z;
		return *this;
	}

	Vec3 operator*(float d) const {
		return Vec3(X * d, Y * d, Z * d);
	}

	Vec3 operator+(const Vec3& r) const {
		return Vec3(X + r.X, Y + r.Y, Z + r.Z);
	}

	bool operator ==(const Vec3& r) const {
		return X == r.X && Y == r.Y && Z == r.Z;
	}

	Vec3() : X(0), Y(0), Z(0) {}

	Vec3(float nx, float ny, float nz) : X(nx), Y(ny), Z(nz) {}

	Vec3(float v) : X(v), Y(v), Z(v) {}

};

inline Vec3 operator-(const Vec3& l, const Vec3& r)
{
	Vec3 Ret(l.X - r.X, l.Y - r.Y, l.Z - r.Z);

	return Ret;
}

inline Vec3 operator/(const Vec3& l, const float& d)
{
	Vec3 Ret(l.X / d, l.Y / d, l.Z / d);

	return Ret;
}
inline Vec3 operator*(const Vec3& l, const Vec3& r)
{
	Vec3 Ret(l.X * r.X, l.Y * r.Y, l.Z * r.Z);

	return Ret;
}
inline Vec3 operator/(const Vec3& l, const Vec3& r)
{
	Vec3 Ret(l.X / r.X, l.Y / r.Y, l.Z / r.Z);

	return Ret;
}
inline bool operator==(const Vec3& l, const Vec3& r)
{
	bool result = false;
	if (REAL_NUM_EQ(l.X, r.X) && REAL_NUM_EQ(l.Y, r.Y) && REAL_NUM_EQ(l.Z, r.Z)) result = true;

	return result;
}
inline bool operator!=(const Vec3& l, const Vec3& r)
{
	bool result = false;
	if (!REAL_NUM_EQ(l.X, r.X) || !REAL_NUM_EQ(l.Y, r.Y) || !REAL_NUM_EQ(l.Z, r.Z)) result = true;

	return result;
}

Vec3 vCross(const Vec3& v1, const Vec3& v2);
Vec3 vmul(Vec3& v, float f);
Vec3 vadd(Vec3& v1, Vec3& v2);
Vec3 quatToEuler(const ZSQUATERNION& q);
Vec3 lerp(Vec3 v1, Vec3 v2, float factor);
float getDistance(const Vec3& p1, const Vec3& p2);
Vec3 _getDirection(const Vec3& rot);
Vec3 _getDirection(float pitch, float yaw, float roll);
float vDot(const Vec3& v1, const Vec3& v2);
bool isDistanceFits(const Vec3& pos1, const Vec3& pos2, float max_dist);