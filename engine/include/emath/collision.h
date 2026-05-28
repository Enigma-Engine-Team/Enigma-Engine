#pragma once

#include "aabb.h"
#include "ray.h"
#include "triangle.h"
#include "frustrum.h"

namespace Math
{
	class Collision
	{
	public:
		static bool CheckAABBRay(const Ray& ray, const AABB& aabb, float& tOut);
		static bool CheckTriangleRay(const Ray& ray, const Triangle& triangle, float& tOut);
		static bool CheckFrustumAABB(const AABB& aabb, const Frustum& frustum);

	private:

		static bool CheckFrustumPlane(const Plane& plane, const Math::AABB& aabb);
	};
}