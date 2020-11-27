#include "../../headers/math/Vec3.hpp"
#include "../../headers/render/Math.hpp"

Vec3 vCross(const Vec3& v1, const Vec3& v2)
{
	return Vec3(v1.Y * v2.Z - v1.Z * v2.Y, v1.Z * v2.X - v1.X * v2.Z, v1.X * v2.Y - v1.Y * v2.X);
}

float vDot(const Vec3& v1, const Vec3& v2)
{
	return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z;
}

Vec3 lerp(Vec3 v1, Vec3 v2, float factor) {
    Vec3 result = v1 * (1.f - factor) + v2 * factor;
    return result;
}

Vec3 vmul(Vec3& v, float f) {
    Vec3 Ret(v.X * f, v.Y * f, v.Z * f);
    return Ret;
}
Vec3 vadd(Vec3& v1, Vec3& v2) {
    Vec3 Ret(v1.X + v2.X, v1.Y + v2.Y, v1.Z + v2.Z);
    return Ret;
}

Vec3 quatToEuler(const ZSQUATERNION& q) {
    return _getDirection(q.X, q.Y, q.Z);
}

float getDistance(const Vec3& p1, const Vec3& p2) {
    float dx = p1.X - p2.X;
    float dy = p1.Y - p2.Y;
    float dz = p1.Z - p2.Z;

    return static_cast<float>(sqrtf(dx * dx + dy * dy + dz * dz));
}

Vec3 _getDirection(float pitch, float yaw, float roll) {
    Vec3 q;

    float cy = cosf(DegToRad(yaw * 0.5f));
    float sy = sinf(DegToRad(yaw * 0.5f));
    float cr = cosf(DegToRad(roll * 0.5f));
    float sr = sinf(DegToRad(roll * 0.5f));
    float cp = cosf(DegToRad(pitch * 0.5f));
    float sp = sinf(DegToRad(pitch * 0.5f));


    q.Z = (cy * sr * cp - sy * cr * sp);
    q.X = (cy * cr * sp + sy * sr * cp);
    q.Y = (sy * cr * cp - cy * sr * sp);

    return q;
}