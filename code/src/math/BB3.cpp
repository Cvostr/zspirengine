#include "../../headers/math/BB3.hpp"

#define MAX_DIST 1000000.f

void BoundingBox3::Extend(const BoundingBox3& Box) {
	if (Box.mMin.X < mMin.X)
		mMin.X = Box.mMin.X;
	if (Box.mMin.Y < mMin.Y)
		mMin.Y = Box.mMin.Y;
	if (Box.mMin.Z < mMin.Z)
		mMin.Z = Box.mMin.Z;

	if (Box.mMax.X > mMax.X)
		mMax.X = Box.mMax.X;
	if (Box.mMax.Y > mMax.Y)
		mMax.Y = Box.mMax.Y;
	if (Box.mMax.Z > mMax.Z)
		mMax.Z = Box.mMax.Z;
}

void BoundingBox3::PrepareForExtend() {
	mMin = Vec3(MAX_DIST);
	mMax = Vec3(-MAX_DIST);
}

void BoundingBox3::Extend(const Vec3& Vec) {
	if (mMin.X > Vec.X) mMin.X = Vec.X;
	if (mMin.Y > Vec.Y) mMin.Y = Vec.Y;
	if (mMin.Z > Vec.Z) mMin.Z = Vec.Z;

	if (mMax.X < Vec.X) mMax.X = Vec.X;
	if (mMax.Y < Vec.Y) mMax.Y = Vec.Y;
	if (mMax.Z < Vec.Z) mMax.Z = Vec.Z;
}

bool BoundingBox3::IsIntersects(const BoundingBox3& Box) {
	if ((this->mMax.X < Box.mMin.X) || (this->mMin.X > Box.mMax.X) ||
		(this->mMax.Y < Box.mMin.Y) || (this->mMin.Y > Box.mMax.Y) ||
		(this->mMax.Z < Box.mMin.Z) || (this->mMin.Z > Box.mMax.Z))
	{
		return false;
	}
	return true;
}
Vec3 BoundingBox3::GetCenter() {
	return (mMin + mMax) * 0.5f;
}

Vec3 BoundingBox3::GetSize() {
	return (mMax - mMin) * 0.5;
}

Mat4 BoundingBox3::GetMatrix() {
	return getScaleMat(GetSize()) * getTranslationMat(GetCenter());
}

void BoundingBox3::CreateFromVertexArray(ZSVERTEX* Array, unsigned int VertexCount) {
	PrepareForExtend();
	for (unsigned int v_i = 0; v_i < VertexCount; v_i++) {
		Extend(Array[v_i].pos);
	}
}

void BoundingBox3::ApplyTransform(const Vec3& pos, const Vec3& scale, const Vec3& rot) {

}

void BoundingBox3::ApplyTransform(const Mat4& Transform) {
	Vec3 corners[8];
	corners[0] = mMin;
	corners[1] = Vec3(mMin.X, mMax.Y, mMin.Z);
	corners[2] = Vec3(mMin.X, mMax.Y, mMax.Z);
	corners[3] = Vec3(mMin.X, mMin.Y, mMax.Z);
	corners[4] = Vec3(mMax.X, mMin.Y, mMin.Z);
	corners[5] = Vec3(mMax.X, mMax.Y, mMin.Z);
	corners[6] = mMax;			   		  
	corners[7] = Vec3(mMax.X, mMin.Y, mMax.Z);

	PrepareForExtend();
	for (int i = 0; i < 8; i++) {
		Vec4 v4 = Vec4(corners[i], 1.0f);
		v4 = matrixVecMM(Transform, v4);
		Extend(v4._Vec3());
	}
}

float getDistanceXZ(const Vec3& p1, const Vec3& p2) {
	float dx = p1.X - p2.X;
	float dz = p1.Z - p2.Z;

	return static_cast<float>(sqrtf(dx * dx + dz * dz));
}

float BoundingBox3::GetLongestDistance(const Vec3& Point) {

	float MaxDist = 0;

	Vec3 corners[8];
	corners[0] = mMin;
	corners[1] = Vec3(mMin.X, mMax.Y, mMin.Z);
	corners[2] = Vec3(mMin.X, mMax.Y, mMax.Z);
	corners[3] = Vec3(mMin.X, mMin.Y, mMax.Z);
	corners[4] = Vec3(mMax.X, mMin.Y, mMin.Z);
	corners[5] = Vec3(mMax.X, mMax.Y, mMin.Z);
	corners[6] = mMax;
	corners[7] = Vec3(mMax.X, mMin.Y, mMax.Z);

	for (int i = 0; i < 8; i++) {
		float Dist = getDistance(Point, corners[i]);
		if (Dist > MaxDist)
			MaxDist = Dist;
	}


	return MaxDist;
}