#pragma once

class Vec2 {
public:
	float X;
	float Y;

	Vec2() : X(0), Y(0) {}
	Vec2(float nx, float ny) : X(nx), Y(ny) {}
	Vec2(float v) : X(v), Y(v) {}

};

inline Vec2 operator-(const Vec2& l, const Vec2& r)
{
	Vec2 Ret(l.X - r.X, l.Y - r.Y);

	return Ret;
}