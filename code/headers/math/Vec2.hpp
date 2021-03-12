#pragma once

class Vec2 {
public:
	float X;
	float Y;

	Vec2 operator+(const Vec2& r) const {
		return Vec2(X + r.X, Y + r.Y);
	}

	Vec2 operator-(const Vec2& r) const {
		return Vec2(X - r.X, Y - r.Y);
	}

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

class Vec2i {
public:
	int X;
	int Y;

	Vec2i operator-(const Vec2i& r) const {
		return Vec2i(X - r.X, Y - r.Y);
	}

	Vec2i operator*(int d) const {
		return Vec2i(X * d, Y * d);
	}

	Vec2i& operator*=(const int& d) {
		X *= d;
		Y *= d;
		return *this;
	}

	Vec2i operator/(int d) const {
		return Vec2i(X / d, Y / d);
	}

	Vec2i& operator/=(const int& d) {
		X /= d;
		Y /= d;
		return *this;
	}

	Vec2i() : X(0), Y(0) {}
	Vec2i(int nx, int ny) : X(nx), Y(ny) {}
	Vec2i(int v) : X(v), Y(v) {}

};