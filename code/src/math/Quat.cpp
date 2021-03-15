#include <math/Quat.hpp>

Quaternion slerp(Quaternion q1, Quaternion q2, float factor) {
    Quaternion result;
    //Normalize quaternions
    q1.Normalize();
    q2.Normalize();

    float dot_product = result.X * result.X + result.Y * result.Y + result.Z * result.Z + result.W * result.W;
    float one_minus_blend = 1.0f - factor;

    if (dot_product < 0.0f)
    {
        result.X = q1.X * one_minus_blend + factor * -q2.X;
        result.Y = q1.Y * one_minus_blend + factor * -q2.Y;
        result.Z = q1.Z * one_minus_blend + factor * -q2.Z;
        result.W = q1.W * one_minus_blend + factor * -q2.W;
    }
    else
    {
        result.X = q1.X * one_minus_blend + factor * q2.X;
        result.Y = q1.Y * one_minus_blend + factor * q2.Y;
        result.Z = q1.Z * one_minus_blend + factor * q2.Z;
        result.W = q1.W * one_minus_blend + factor * q2.W;
    }

    result.Normalize();

    return result;
}