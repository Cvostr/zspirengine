#include <math/Frustum.hpp>
#include <render/Math.hpp>

void Engine::Frustum::Create(float Fov, float AspectRatio, float nearZ, float farZ, Mat4& Transform) {
	Vec3 Near;
	Vec3 Far;

	float halfViewSize = tanf(DegToRad(Fov) / 2);

	Near.Z = nearZ;
	Near.Y = nearZ * halfViewSize;
	Near.X = Near.Y * AspectRatio;

	Far.Z = farZ;
	Far.Y = Far.Z * halfViewSize;
	Far.X = Far.Y * AspectRatio;

	Create(Near, Far, Transform);
}

void Engine::Frustum::Create(Vec3 Near, Vec3 Far, Mat4& Transform) {
	mFrustumVertices[0] = (Transform * Vec4(Near, 1))._Vec3();
	mFrustumVertices[1] = (Transform * Vec4(Near.X, -Near.Y, Near.Z, 1))._Vec3();
	mFrustumVertices[2] = (Transform * Vec4(-Near.X, -Near.Y, Near.Z, 1))._Vec3();
	mFrustumVertices[3] = (Transform * Vec4(-Near.X, Near.Y, Near.Z, 1))._Vec3();
	mFrustumVertices[4] = (Transform * Vec4(Far, 1))._Vec3();
	mFrustumVertices[5] = (Transform * Vec4(Far.X, -Far.Y, Far.Z, 1))._Vec3();
	mFrustumVertices[6] = (Transform * Vec4(-Far.X, -Far.Y, Far.Z, 1))._Vec3();
	mFrustumVertices[7] = (Transform * Vec4(-Far.X, Far.Y, Far.Z, 1))._Vec3();

	mFrustumPlanes[PLANE_NEAR].Create(mFrustumVertices[2], mFrustumVertices[1], mFrustumVertices[0]);
	mFrustumPlanes[PLANE_LEFT].Create(mFrustumVertices[3], mFrustumVertices[7], mFrustumVertices[6]);
	mFrustumPlanes[PLANE_RIGHT].Create(mFrustumVertices[1], mFrustumVertices[5], mFrustumVertices[4]);
	mFrustumPlanes[PLANE_UP].Create(mFrustumVertices[0], mFrustumVertices[4], mFrustumVertices[7]);
	mFrustumPlanes[PLANE_DOWN].Create(mFrustumVertices[6], mFrustumVertices[5], mFrustumVertices[1]);
	mFrustumPlanes[PLANE_FAR].Create(mFrustumVertices[5], mFrustumVertices[6], mFrustumVertices[7]);

	if (mFrustumPlanes[PLANE_NEAR].Distance(mFrustumVertices[5]) < 0.0f)
	{
		for (auto& plane : mFrustumPlanes)
		{
			plane.Create(plane.GetNormal() * -1, -plane.GetD());

		}
	}
}

void Engine::Frustum::Create(const Mat4& projection, const Mat4& View)
{/*
	Mat4 TransposedView = View;
	TransposedView.Transpose();
	Mat4 Matrix = projection * View;
	Matrix.Transpose();

	//LEFT
	Vec3 Plane0Normal = Vec3(
		Matrix.m[3][0] + Matrix.m[0][0],
		Matrix.m[3][1] + Matrix.m[0][1],
		Matrix.m[3][2] + Matrix.m[0][2]
	);

	float Plane0D = Matrix.m[3][3] + Matrix.m[0][3];
	// Right clipping plane
	Vec3 Plane1Normal = Vec3(
		Matrix.m[3][0] - Matrix.m[0][0],
		Matrix.m[3][1] - Matrix.m[0][1],
		Matrix.m[3][2] - Matrix.m[0][2]
	);

	float Plane1D = Matrix.m[3][3] - Matrix.m[0][3];

	// Top clipping plane
	Vec3 Plane2Normal = Vec3(
		Matrix.m[3][0] - Matrix.m[1][0],
		Matrix.m[3][1] - Matrix.m[1][1],
		Matrix.m[3][2] - Matrix.m[1][2]
	);

	float Plane2D = Matrix.m[3][3] - Matrix.m[1][3];

	// Bottom clipping plane
	Vec3 Plane3Normal = Vec3(
		Matrix.m[3][0] + Matrix.m[1][0],
		Matrix.m[3][1] + Matrix.m[1][1],
		Matrix.m[3][2] + Matrix.m[1][2]
	);

	float Plane3D = Matrix.m[3][3] + Matrix.m[1][3];

	// Near clipping plane
	Vec3 Plane4Normal = Vec3(
		Matrix.m[3][0] + Matrix.m[2][0],
		Matrix.m[3][1] + Matrix.m[2][1],
		Matrix.m[3][2] + Matrix.m[2][2]
	);

	float Plane4D = Matrix.m[3][3] + Matrix.m[2][3];
	
	// Far clipping plane
	Vec3 Plane5Normal = Vec3(
		Matrix.m[3][0] - Matrix.m[2][0],
		Matrix.m[3][1] - Matrix.m[2][1],
		Matrix.m[3][2] - Matrix.m[2][2]
	);

	float Plane5D = Matrix.m[3][3] - Matrix.m[2][3];

	mFrustumPlanes[PLANE_LEFT].Create(Plane0Normal, Plane0D);
	mFrustumPlanes[PLANE_RIGHT].Create(Plane1Normal, Plane1D);

	mFrustumPlanes[PLANE_UP].Create(Plane2Normal, Plane2D);
	mFrustumPlanes[PLANE_DOWN].Create(Plane3Normal, Plane3D);

	mFrustumPlanes[PLANE_NEAR].Create(Plane4Normal, Plane4D);
	mFrustumPlanes[PLANE_FAR].Create(Plane5Normal, Plane5D);
	*/
	Mat4 projInverse = projection * View;
	//projInverse.Invert();

	mFrustumVertices[0] = (projInverse * Vec4(1.0f, 1.0f, 0.0f, 1))._Vec3();
	mFrustumVertices[1] = (projInverse * Vec4(1.0f, -1.0f, 0.0f, 1))._Vec3();
	mFrustumVertices[2] = (projInverse * Vec4(-1.0f, -1.0f, 0.0f, 1))._Vec3();
	mFrustumVertices[3] = (projInverse * Vec4(-1.0f, 1.0f, 0.0f, 1))._Vec3();
	mFrustumVertices[4] = (projInverse * Vec4(1.0f, 1.0f, 1.0f, 1))._Vec3();
	mFrustumVertices[5] = (projInverse * Vec4(1.0f, -1.0f, 1.0f, 1))._Vec3();
	mFrustumVertices[6] = (projInverse * Vec4(-1.0f, -1.0f, 1.0f, 1))._Vec3();
	mFrustumVertices[7] = (projInverse * Vec4(-1.0f, 1.0f, 1.0f, 1))._Vec3();

	mFrustumPlanes[PLANE_NEAR].Create(mFrustumVertices[2], mFrustumVertices[1], mFrustumVertices[0]);
	mFrustumPlanes[PLANE_LEFT].Create(mFrustumVertices[3], mFrustumVertices[7], mFrustumVertices[6]);
	mFrustumPlanes[PLANE_RIGHT].Create(mFrustumVertices[1], mFrustumVertices[5], mFrustumVertices[4]);
	mFrustumPlanes[PLANE_UP].Create(mFrustumVertices[0], mFrustumVertices[4], mFrustumVertices[7]);
	mFrustumPlanes[PLANE_DOWN].Create(mFrustumVertices[6], mFrustumVertices[5], mFrustumVertices[1]);
	mFrustumPlanes[PLANE_FAR].Create(mFrustumVertices[5], mFrustumVertices[6], mFrustumVertices[7]);

	if (mFrustumPlanes[PLANE_NEAR].Distance(mFrustumVertices[5]) < 0.0f)
	{
		for (auto& plane : mFrustumPlanes)
		{
			plane.Create(plane.GetNormal() * -1, -plane.GetD());
		
		}
	}
}

Engine::FrustumRelation Engine::Frustum::GetRelation(Vec3& Point) {
	for (unsigned int plane_i = 0; plane_i < 6; plane_i++) {
		Plane* plane_ptr = &mFrustumPlanes[plane_i];

		if (plane_ptr->Distance(Point) < 0)
			return FR_OUTSIDE;

	}

	return FR_INSIDE;
}
Engine::FrustumRelation Engine::Frustum::GetRelation(BoundingBox3& BB) {
	Vec3 center = BB.GetCenter();
	Vec3 edge = center - BB.mMin;
	bool allInside = true;

	for (unsigned int plane_i = 0; plane_i < 6; plane_i++) 
	{
		Plane* plane_ptr = &mFrustumPlanes[plane_i];
		Vec3 PlaneNormal = plane_ptr->GetNormal();
		Vec3 PlaneNormalAbs = Vec3(fabs(PlaneNormal.X), fabs(PlaneNormal.Y), fabs(PlaneNormal.Z));
		PlaneNormalAbs.Normalize();

		float dist = vDot(PlaneNormal, center) + plane_ptr->GetD();
		float absDist = vDot(PlaneNormalAbs, edge);

		if (dist < -absDist)
			return FR_OUTSIDE;
		else if (dist < absDist)
			allInside = false;
	}

	return allInside ? FR_INSIDE : FR_INTERSECTS;
}