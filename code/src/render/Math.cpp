#include "../../headers/render/Math.hpp"


float DegToRad(float degrees){
	return degrees * ZS_PI / 180;
}

void vNormalize(ZSVECTOR3* v)
{
	ZSVECTOR3 result;

	float sqr = v->X * v->X + v->Y * v->Y + v->Z * v->Z;
	if (REAL_NUM_EQ(sqr, 1) || REAL_NUM_EQ(sqr, 0))
		return;

    float invrt = (1.0f / static_cast<float>(sqrt(sqr)));
	v->X *= invrt;
	v->Y *= invrt;
	v->Z *= invrt;

}




bool isDistanceFits(const ZSVECTOR3& pos1, const ZSVECTOR3& pos2, float max_dist){
    float dist = getDistance(pos1, pos2);
    return dist <= max_dist ? true : false;
}



ZSQUATERNION slerp(ZSQUATERNION q1, ZSQUATERNION q2, float factor){
    ZSQUATERNION result;
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

void ZSRGBCOLOR::updateGL() {
    this->gl_r = static_cast<float>(r) / 255;
    this->gl_g = static_cast<float>(g) / 255;
    this->gl_b = static_cast<float>(b) / 255;
    this->gl_a = static_cast<float>(a) / 255;
}

