#pragma once

#include "Vec3.hpp"
#include "Mat4.hpp"
#include "Vertex.hpp"

class BoundingBox3 {
public:
	ZSVECTOR3 mMin;
	ZSVECTOR3 mMax;

	void Extend(const BoundingBox3& Box);
	void Extend(const ZSVECTOR3& Vec);
	void PrepareForExtend();
	bool IsIntersects(const BoundingBox3& Box);
	ZSVECTOR3 GetCenter();
	void CreateFromVertexArray(ZSVERTEX* Array, unsigned int VertexCount);
	void ApplyTransform(const ZSVECTOR3& pos, const ZSVECTOR3& scale, const ZSVECTOR3& rot);
	void ApplyTransform(const Mat4& Transform);

	BoundingBox3(const ZSVECTOR3& Min, const ZSVECTOR3& Max) : mMin(Min), mMax(Max){}
	BoundingBox3() {
		PrepareForExtend();
	}
};
