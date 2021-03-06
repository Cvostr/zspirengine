#include "../../headers/math/Mat4.hpp"
#include "../../headers/render/Math.hpp"
#include "../../headers/game.h"

extern ZSpireEngine* engine_ptr;

Mat4 getIdentity() {
    Mat4 Ret;

    Ret.m[0][0] = 1; Ret.m[0][1] = 0; Ret.m[0][2] = 0; Ret.m[0][3] = 0;
    Ret.m[1][0] = 0; Ret.m[1][1] = 1; Ret.m[1][2] = 0; Ret.m[1][3] = 0;
    Ret.m[2][0] = 0; Ret.m[2][1] = 0; Ret.m[2][2] = 1; Ret.m[2][3] = 0;
    Ret.m[3][0] = 0; Ret.m[3][1] = 0; Ret.m[3][2] = 0; Ret.m[3][3] = 1;

    return Ret;
}

Vec4 matrixVecMM(const Mat4& l, const Vec4& r) {
    Vec4 Ret;
    
    Ret.X = r.X * l.m[0][0] + r.Y * l.m[1][0] + r.Z * l.m[2][0] + r.W * l.m[3][0];
    Ret.Y = r.X * l.m[0][1] + r.Y * l.m[1][1] + r.Z * l.m[2][1] + r.W * l.m[3][1];
    Ret.Z = r.X * l.m[0][2] + r.Y * l.m[1][2] + r.Z * l.m[2][2] + r.W * l.m[3][2];
    Ret.W = r.X * l.m[0][3] + r.Y * l.m[1][3] + r.Z * l.m[2][3] + r.W * l.m[3][3];

    return Ret;
}

Mat4 matrixMM(const Mat4& l, const Mat4& r) {
    Mat4 Ret;
    for (unsigned int i = 0; i < 4; i++) {
        for (unsigned int b = 0; b < 4; b++) {
            Ret.m[i][b] = l.m[i][0] * r.m[0][b] +
                l.m[i][1] * r.m[1][b] +
                l.m[i][2] * r.m[2][b] +
                l.m[i][3] * r.m[3][b];
        }
    }
    return Ret;
}

Mat4 matrixSum(const Mat4& l, const Mat4& r) {
    Mat4 Ret;
    for (unsigned int i = 0; i < 4; i++) {
        for (unsigned int b = 0; b < 4; b++) {
            Ret.m[i][b] = l.m[i][b] + r.m[i][b];
        }
    }
    return Ret;
}


Mat4 getPerspective(float fovy, float aspect, float zNear, float zFar) {
    Mat4 result;

    double range = tan(DegToRad(fovy) / 2);

    result.m[0][0] = (1.f) / static_cast<float>(aspect * range);
    result.m[1][1] = (1.f) / static_cast<float>(range);
    result.m[2][2] = -(zFar + zNear) / (zFar - zNear);
    result.m[2][3] = -1.0f;
    result.m[3][2] = -(2.0f * zFar * zNear) / (zFar - zNear);

    return result;
}

Mat4 getPerspectiveVulkan(float fovy, float aspect, float zNear, float zFar) {
    Mat4 result;

    double range = tan(DegToRad(fovy) / 2);

    result.m[0][0] = (1.f) / static_cast<float>(aspect * range);
    result.m[1][1] = (-1.f) / static_cast<float>(range);
    result.m[2][2] = zFar / (zNear - zFar);
    result.m[2][3] = -1.0f;
    result.m[3][2] = -(zFar * zNear) / (zFar - zNear);

    return result;
}

Mat4 transpose(const Mat4& mat) {
    Mat4 result;

    for (unsigned int i = 0; i < 4; i++) {
        for (unsigned int j = 0; j < 4; j++) {
            result.m[i][j] = mat.m[j][i];
        }
    }
    return result;
}

float determinant(const Mat4& mat) {
    //[0][0] [0][1] [0][2] [0][3]
    //[1][0] [1][1] [1][2] [1][3]
    //[2][0] [2][1] [2][2] [2][3]
    //[3][0] [3][1] [3][2] [3][3]

    float d1 = mat.m[0][0] * determinant(mat.m[1][1], mat.m[1][2], mat.m[1][3], mat.m[2][1], mat.m[2][2], mat.m[2][3], mat.m[3][1], mat.m[3][2], mat.m[3][3]);
    float d2 = mat.m[0][1] * determinant(mat.m[1][0], mat.m[1][2], mat.m[1][3], mat.m[2][0], mat.m[2][2], mat.m[2][3], mat.m[3][0], mat.m[3][2], mat.m[3][3]);
    float d3 = mat.m[0][2] * determinant(mat.m[1][0], mat.m[1][1], mat.m[1][3], mat.m[2][0], mat.m[2][1], mat.m[2][3], mat.m[3][0], mat.m[3][1], mat.m[3][3]);
    float d4 = mat.m[0][3] * determinant(mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[3][0], mat.m[3][1], mat.m[3][2]);

    return d1 - d2 + d3 - d4;

}
float determinant(float a, float b, float c, float d, float e, float f, float g, float h, float i) {
    //a b c
    //d e f
    //g h i
    return (a * e * i) + (d * c * h) + (g * b * f) - (g * e * c) - (d * b * i) - (h * f * a);
}

Mat4 invert(const Mat4& mat) {
    float _determinant = determinant(mat);
    _determinant = 1.f / _determinant;

    float A11 = +determinant(mat.m[1][1], mat.m[1][2], mat.m[1][3], mat.m[2][1], mat.m[2][2], mat.m[2][3], mat.m[3][1], mat.m[3][2], mat.m[3][3]);
    float A12 = -determinant(mat.m[1][0], mat.m[1][2], mat.m[1][3], mat.m[2][0], mat.m[2][2], mat.m[2][3], mat.m[3][0], mat.m[3][2], mat.m[3][3]);
    float A13 = +determinant(mat.m[1][0], mat.m[1][1], mat.m[1][3], mat.m[2][0], mat.m[2][1], mat.m[2][3], mat.m[3][0], mat.m[3][1], mat.m[3][3]);
    float A14 = -determinant(mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[3][0], mat.m[3][1], mat.m[3][2]);

    float A21 = -determinant(mat.m[0][1], mat.m[0][2], mat.m[0][3], mat.m[2][1], mat.m[2][2], mat.m[2][3], mat.m[3][1], mat.m[3][2], mat.m[3][3]);
    float A22 = +determinant(mat.m[0][0], mat.m[0][2], mat.m[0][3], mat.m[2][0], mat.m[2][2], mat.m[2][3], mat.m[3][0], mat.m[3][2], mat.m[3][3]);
    float A23 = -determinant(mat.m[0][0], mat.m[0][1], mat.m[0][3], mat.m[2][0], mat.m[2][1], mat.m[2][3], mat.m[3][0], mat.m[3][1], mat.m[3][3]);
    float A24 = +determinant(mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[3][0], mat.m[3][1], mat.m[3][2]);

    float A31 = +determinant(mat.m[0][1], mat.m[0][2], mat.m[0][3], mat.m[1][1], mat.m[1][2], mat.m[1][3], mat.m[3][1], mat.m[3][2], mat.m[3][3]);
    float A32 = -determinant(mat.m[0][0], mat.m[0][2], mat.m[0][3], mat.m[1][0], mat.m[1][2], mat.m[1][3], mat.m[3][0], mat.m[3][2], mat.m[3][3]);
    float A33 = +determinant(mat.m[0][0], mat.m[0][1], mat.m[0][3], mat.m[1][0], mat.m[1][1], mat.m[1][3], mat.m[3][0], mat.m[3][1], mat.m[3][3]);
    float A34 = -determinant(mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[3][0], mat.m[3][1], mat.m[3][2]);

    float A41 = -determinant(mat.m[0][1], mat.m[0][2], mat.m[0][3], mat.m[1][1], mat.m[1][2], mat.m[1][3], mat.m[2][1], mat.m[2][2], mat.m[2][3]);
    float A42 = +determinant(mat.m[0][0], mat.m[0][2], mat.m[0][3], mat.m[1][0], mat.m[1][2], mat.m[1][3], mat.m[2][0], mat.m[2][2], mat.m[2][3]);
    float A43 = -determinant(mat.m[0][0], mat.m[0][1], mat.m[0][3], mat.m[1][0], mat.m[1][1], mat.m[1][3], mat.m[2][0], mat.m[2][1], mat.m[2][3]);
    float A44 = +determinant(mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[2][0], mat.m[2][1], mat.m[2][2]);

    Mat4 result = Mat4(Vec4(A11, A21, A31, A41), Vec4(A12, A22, A32, A42), Vec4(A13, A23, A33, A43),
        Vec4(A14, A24, A34, A44));
    result = result * _determinant;
    return result;
}

Mat4 matrixLookAt(const Vec3& eye, const Vec3& center, const Vec3& up)
{
    Mat4 out;

    Vec3 f = Vec3(center.X - eye.X, center.Y - eye.Y, center.Z - eye.Z);
    f.Normalize();

    Vec3 u;

    Vec3 s = vCross(f, up);
    s.Normalize();
    u = vCross(s, f);

    out = getIdentity();
    out.m[0][0] = s.X;
    out.m[1][0] = s.Y;
    out.m[2][0] = s.Z;

    out.m[0][1] = u.X;
    out.m[1][1] = u.Y;
    out.m[2][1] = u.Z;

    out.m[0][2] = -f.X;
    out.m[1][2] = -f.Y;
    out.m[2][2] = -f.Z;

    out.m[3][0] = -vDot(s, eye);
    out.m[3][1] = -vDot(u, eye);
    out.m[3][2] = vDot(f, eye);

    return out;
}

Mat4 removeTranslationFromViewMat(Mat4 viewMat) {
    Mat4 result = viewMat;

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = 0;
    result.m[3][3] = 1;
    result.m[0][3] = 0;
    result.m[1][3] = 0;
    result.m[2][3] = 0;

    return result;
}
Mat4 removeRotationFromTransformMat(Mat4 transform, Mat4 view) {
    Mat4 result = transform;

    result.m[0][0] = view.m[0][0];
    result.m[0][1] = view.m[1][0];
    result.m[0][2] = view.m[2][0];

    result.m[1][0] = view.m[0][1];
    result.m[1][1] = view.m[1][1];
    result.m[1][2] = view.m[2][1];

    result.m[2][0] = view.m[0][2];
    result.m[2][1] = view.m[1][2];
    result.m[2][2] = view.m[2][2];

    return result;
}

Mat4 getScaleMat(float scaleX, float scaleY, float scaleZ) {

    Mat4 mat;

    mat.m[0][0] = scaleX; mat.m[0][1] = 0; mat.m[0][2] = 0; mat.m[0][3] = 0;
    mat.m[1][0] = 0; mat.m[1][1] = scaleY; mat.m[1][2] = 0; mat.m[1][3] = 0;
    mat.m[2][0] = 0; mat.m[2][1] = 0; mat.m[2][2] = scaleZ; mat.m[2][3] = 0;
    mat.m[3][0] = 0; mat.m[3][1] = 0; mat.m[3][2] = 0; mat.m[3][3] = 1;

    return mat;
}

Mat4 getScaleMat(const Vec3& scale) {
    return getScaleMat(scale.X, scale.Y, scale.Z);
}

Mat4 getTranslationMat(float trX, float trY, float trZ) {

    Mat4 mat;

    mat.m[0][0] = 1; mat.m[0][1] = 0; mat.m[0][2] = 0; mat.m[0][3] = 0;
    mat.m[1][0] = 0; mat.m[1][1] = 1; mat.m[1][2] = 0; mat.m[1][3] = 0;
    mat.m[2][0] = 0; mat.m[2][1] = 0; mat.m[2][2] = 1; mat.m[2][3] = 0;
    mat.m[3][0] = trX; mat.m[3][1] = trY; mat.m[3][2] = trZ; mat.m[3][3] = 1;

    return mat;
}

Mat4 getTranslationMat(const Vec3& translation) {
    return getTranslationMat(translation.X, translation.Y, translation.Z);
}

Mat4 getRotationXMat(float thetaN) {
    float theta = DegToRad(thetaN);
    Mat4 mat;

    mat.m[0][0] = 1; mat.m[0][1] = 0;               mat.m[0][2] = 0;                mat.m[0][3] = 0;
    mat.m[1][0] = 0; mat.m[1][1] = static_cast<float>(cos(theta));      mat.m[1][2] = static_cast<float>(sin(theta)) * -1;  mat.m[1][3] = 0;
    mat.m[2][0] = 0; mat.m[2][1] = static_cast<float>(sin(theta));      mat.m[2][2] = static_cast<float>(cos(theta));       mat.m[2][3] = 0;
    mat.m[3][0] = 0; mat.m[3][1] = 0;               mat.m[3][2] = 0;                mat.m[3][3] = 1;

    return mat;
}
Mat4 getRotationYMat(float thetaN) {
    float theta = DegToRad(thetaN);
    Mat4 mat;

    mat.m[0][0] = static_cast<float>(cos(theta));       mat.m[0][1] = 0;            mat.m[0][2] = static_cast<float>(sin(theta));   mat.m[0][3] = 0;
    mat.m[1][0] = 0;                mat.m[1][1] = 1;            mat.m[1][2] = 0;            mat.m[1][3] = 0;
    mat.m[2][0] = static_cast<float>(sin(theta)) * -1;  mat.m[2][1] = 0;            mat.m[2][2] = static_cast<float>(cos(theta));   mat.m[2][3] = 0;
    mat.m[3][0] = 0;                mat.m[3][1] = 0;            mat.m[3][2] = 0;            mat.m[3][3] = 1;

    return mat;
}
Mat4 getRotationZMat(float thetaN) {
    float theta = DegToRad(thetaN);
    Mat4 mat;

    mat.m[0][0] = static_cast<float>(cos(theta));       mat.m[0][1] = static_cast<float>(sin(theta)) * -1;  mat.m[0][2] = 0;   mat.m[0][3] = 0;
    mat.m[1][0] = static_cast<float>(sin(theta));       mat.m[1][1] = static_cast<float>(cos(theta));       mat.m[1][2] = 0;   mat.m[1][3] = 0;
    mat.m[2][0] = 0;                mat.m[2][1] = 0;                mat.m[2][2] = 1;   mat.m[2][3] = 0;
    mat.m[3][0] = 0;                mat.m[3][1] = 0;                mat.m[3][2] = 0;   mat.m[3][3] = 1;

    return mat;
}

Mat4 getRotationMat(float thetaX, float thetaY, float thetaZ) {
    //Calculate matrices
    Mat4 xmat = getRotationXMat(thetaX);
    Mat4 ymat = getRotationYMat(thetaY);
    Mat4 zmat = getRotationZMat(thetaZ);

    Mat4 result = getIdentity(); //Define identity
    //Sequantily multiply it by xmat,ymat,zmat
    result = result * xmat;
    result = result * ymat;
    result = result * zmat;

    return result;
}

Mat4 getRotationMat(const Vec3& rotation) {
    return getRotationMat(rotation.X, rotation.Y, rotation.Z);
}

Mat4 getRotationMat(const Vec3& rotation, const Vec3& center) {
    Mat4 result = getTranslationMat(center);
    result = result * getRotationMat(rotation);
    result = result * getTranslationMat(center * -1);
    return result;
}

Mat4 getRotationMat(const Quaternion& quat) {
    float x2 = quat.X * quat.X;
    float y2 = quat.Y * quat.Y;
    float z2 = quat.Z * quat.Z;
    float xy = quat.X * quat.Y;
    float xz = quat.X * quat.Z;
    float yz = quat.Y * quat.Z;
    float wx = quat.W * quat.X;
    float wy = quat.W * quat.Y;
    float wz = quat.W * quat.Z;

    // This calculation would be a lot more complicated for non-unit length quaternions // Note: The constructor of Matrix4 expects the Matrix in column-major format like expected by // OpenGL
    Mat4 rot = Mat4(
        Vec4(1.0f - 2.0f * (y2 + z2), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f), 
        Vec4(2.0f * (xy + wz), 1.0f - 2.0f * (x2 + z2), 2.0f * (yz - wx), 0.0f),
        Vec4(2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (x2 + y2), 0.0f),
        Vec4(0.0f, 0.0f, 0.0f, 1.0f));
    return rot;
}

Mat4 getOrthogonal(float left, float right, float bottom, float top)
{
    Mat4 out = getIdentity();

    out.m[0][0] = 2.0f / (right - left);
    out.m[1][1] = 2.0f / (top - bottom);
    out.m[2][2] = -1.0f;
    out.m[3][0] = -(right + left) / (right - left);
    out.m[3][1] = -(top + bottom) / (top - bottom);

    return out;
}

Mat4 getOrthogonal(float left, float right, float bottom, float top, float zNear, float zFar) {
    Mat4 out = getIdentity();

    /*out.m[0][0] = 2.0f / (right - left);
    out.m[1][1] = 2.0f / (top - bottom);

    out.m[2][2] = -2.0f / (zFar - zNear);
    out.m[3][2] = -(zFar + zNear) / (zFar - zNear);

    out.m[3][0] = -(right + left) / (right - left);
    out.m[3][1] = -(top + bottom) / (top - bottom);*/

    out.m[0][0] = 2.0f / (right - left);
    out.m[1][1] = 2.0f / (top - bottom);

    out.m[2][2] = -1.0f / (zFar - zNear);
    out.m[3][2] = -(zNear) / (zFar - zNear);

    out.m[3][0] = -(right + left) / (right - left);
    out.m[3][1] = -(top + bottom) / (top - bottom);

    return out;
}

Mat4 getOrthogonalVulkan(float left, float right, float bottom, float top, float zNear, float zFar) {
    Mat4 out = getIdentity();

    out.m[0][0] = 2.0f / (right - left);
    out.m[1][1] = -2.0f / (top - bottom);

    out.m[2][2] = -1.0f / (zFar - zNear);
    out.m[3][2] = -(zNear) / (zFar - zNear);

    out.m[3][0] = -(right + left) / (right - left);
    out.m[3][1] = -(top + bottom) / (top - bottom);

    return out;
}

Mat4 GetPlaneReflectionMat(float A, float B, float C, float D) {
    Mat4 out = Mat4(1);

    out.m[0][0] = 1.f - 2.f * A * A;
    out.m[0][1] = -2.f * A * B;
    out.m[0][2] = -2.f * A * C;
    out.m[0][3] = -2.f * A * D;

    out.m[1][0] = -2.f * A * B;
    out.m[1][1] = 1.f - 2.f * B * B;
    out.m[1][2] = -2.f * B * C;
    out.m[1][3] = -2.f * B * D;

    out.m[2][0] = -2.f * A * C;
    out.m[2][1] = -2.f * B * C;
    out.m[2][2] = 1.f - 2.f * C * C;
    out.m[2][3] = -2.f * C * D;

    return out;
}