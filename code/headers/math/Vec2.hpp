#pragma once

class Vec2 {
public:
	float X;
	float Y;

	Vec2 operator*(float d) const {
		return Vec2(X * d, Y * d);
	}

	Vec2& operator*=(const float& d) {
		X *= d;
		Y *= d;
		return *this;
	}

	Vec2 operator/(float d) const {
		return Vec2(X / d, Y / d);
	}

	Vec2& operator/=(const float& d) {
		X /= d;
		Y /= d;
		return *this;
	}

	Vec2() : X(0), Y(0) {}
	Vec2(float nx, float ny) : X(nx), Y(ny) {}
	Vec2(float v) : X(v), Y(v) {}

};

inline Vec2 operator-(const Vec2& l, const Vec2& r)
{
	Vec2 Ret(l.X - r.X, l.Y - r.Y);

	return Ret;
}