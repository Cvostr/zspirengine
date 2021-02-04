#pragma once

#include "Vec3.hpp"
#include "Mat4.hpp"

namespace Engine {
	class Plane {
	private:
		Vec3 mNormal;
		float mD;
	public:
		Plane() :
			mD(0)
		{
		}

		Plane(Vec3 Normal, float D) :
			mNormal(Normal),
			mD(D)
		{
		}

		const Vec3& GetNormal() { return mNormal; }
		float GetD() { return mD; }

		Plane(Vec3 Normal, Vec3 Point);

		Mat4 GetReflectionMatrix();
	};
}