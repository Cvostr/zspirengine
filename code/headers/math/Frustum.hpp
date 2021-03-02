#pragma once

#include "Mat4.hpp"
#include "Plane.hpp"
#include "BB3.hpp"

namespace Engine {

	enum FrustumPlane
	{
		PLANE_NEAR = 0,
		PLANE_LEFT,
		PLANE_RIGHT,
		PLANE_UP,
		PLANE_DOWN,
		PLANE_FAR,
	};

	enum FrustumRelation {
		FR_INSIDE,
		FR_INTERSECTS,
		FR_OUTSIDE
	};

	class Frustum {
	private:
		Plane mFrustumPlanes[6];
		Vec3 mFrustumVertices[8];

	public:

		void Create(float Fov, float AspectRatio, float nearZ, float farZ, Mat4& Transform);
		void Create(Vec3 Near, Vec3 Far, Mat4& Transform);
		void Create(const Mat4& projection);

		FrustumRelation GetRelation(Vec3& Point);
		FrustumRelation GetRelation(BoundingBox3& BB);


		Frustum(){}
	};
}