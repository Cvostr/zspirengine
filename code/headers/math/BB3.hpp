#pragma once

#include "Vec3.hpp"
#include "Mat4.hpp"
#include "Vertex.hpp"

class BoundingBox3 {
public:
	Vec3 mMin;
	Vec3 mMax;

	void Extend(const BoundingBox3& Box);
	void Extend(const Vec3& Vec);
	void PrepareForExtend();
	bool IsIntersects(const BoundingBox3& Box);
	Vec3 GetCenter();
	void CreateFromVertexArray(ZSVERTEX* Array, unsigned int VertexCount);
	void ApplyTransform(const Vec3& pos, const Vec3& scale, const Vec3& rot);
	void ApplyTransform(const Mat4& Transform);

	BoundingBox3(const Vec3& Min, const Vec3& Max) : mMin(Min), mMax(Max){}
	BoundingBox3() {
		PrepareForExtend();
	}
};
