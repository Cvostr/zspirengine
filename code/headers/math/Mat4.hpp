#pragma once 

#include "Vec4.hpp"
#include "Vec3.hpp"
#include "Quat.hpp"

class Mat4 {
public:
	float m[4][4];

	Mat4(ZSVECTOR4 v1, ZSVECTOR4 v2, ZSVECTOR4 v3, ZSVECTOR4 v4) {
		m[0][0] = v1.X; m[0][1] = v1.Y; m[0][2] = v1.Z; m[0][3] = v1.W;
		m[1][0] = v2.X; m[1][1] = v2.Y; m[1][2] = v2.Z; m[1][3] = v2.W;
		m[2][0] = v3.X; m[2][1] = v3.Y; m[2][2] = v3.Z; m[2][3] = v3.W;
		m[3][0] = v4.X; m[3][1] = v4.Y; m[3][2] = v4.Z; m[3][3] = v4.W;


	}
	Mat4() {
		for (unsigned int i = 0; i < 4; i++) {
			for (unsigned int j = 0; j < 4; j++) {
				m[i][j] = 0;
			}
		}

	}

	//float Determinant() { return determinant(*this); }
};

Mat4 getIdentity();
Mat4 transpose(const Mat4& mat);
float determinant(const Mat4& mat);
float determinant(float a, float b, float c, float d, float e, float f, float g, float h, float i);
Mat4 invert(const Mat4& mat);
Mat4 matrixMM(const Mat4& l, const Mat4& r);
ZSVECTOR4 matrixVecMM(const Mat4& l, const ZSVECTOR4& r);
Mat4 matrixSum(const Mat4& l, const Mat4& r);
Mat4 getPerspective(float fovy, float aspect, float zNear, float zFar);
Mat4 getOrthogonal(float left, float right, float bottom, float top);
Mat4 getOrthogonal(float left, float right, float bottom, float top, float zNear, float zFar);
Mat4 matrixLookAt(const ZSVECTOR3& eye, const ZSVECTOR3& center, const ZSVECTOR3& up);
Mat4 removeTranslationFromViewMat(Mat4 viewMat);
Mat4 removeRotationFromTransformMat(Mat4 transform, Mat4 view);

Mat4 getScaleMat(float scaleX, float scaleY, float scaleZ);
Mat4 getScaleMat(const ZSVECTOR3& scale);
Mat4 getTranslationMat(float trX, float trY, float trZ);
Mat4 getTranslationMat(const ZSVECTOR3& translation);
Mat4 getRotationXMat(float thetaN);
Mat4 getRotationYMat(float thetaN);
Mat4 getRotationZMat(float thetaN);
Mat4 getRotationMat(float thetaX, float thetaY, float thetaZ);
Mat4 getRotationMat(const ZSVECTOR3& rotation);
Mat4 getRotationMat(const ZSVECTOR3& rotation, const ZSVECTOR3& center);
Mat4 getRotationMat(const ZSQUATERNION& quat);

inline Mat4 operator*(const Mat4& l, const Mat4& r)
{
	return matrixMM(l, r);

}

inline Mat4 operator*(const Mat4& l, const float& r)
{
	Mat4 result = l;
	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int b = 0; b < 4; b++) {
			result.m[i][b] *= r;
		}
	}

	return l;

}

inline Mat4 operator+(const Mat4& l, const Mat4& r)
{
	return matrixSum(l, r);

}