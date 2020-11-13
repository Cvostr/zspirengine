#pragma once

class ZSVECTOR2 {
public:
	float X;
	float Y;

	ZSVECTOR2() : X(0), Y(0) {}
	ZSVECTOR2(float nx, float ny) : X(nx), Y(ny) {}
	ZSVECTOR2(float v) : X(v), Y(v) {}

};

inline ZSVECTOR2 operator-(const ZSVECTOR2& l, const ZSVECTOR2& r)
{
	ZSVECTOR2 Ret(l.X - r.X, l.Y - r.Y);

	return Ret;
}