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

void ZSRGBCOLOR::updateGL() {
    this->gl_r = static_cast<float>(r) / 255;
    this->gl_g = static_cast<float>(g) / 255;
    this->gl_b = static_cast<float>(b) / 255;
    this->gl_a = static_cast<float>(a) / 255;
}

