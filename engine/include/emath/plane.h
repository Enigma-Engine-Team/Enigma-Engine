#pragma once

#include "vector.h"

namespace Math
{
	struct Plane
	{
		Plane() = default;
		Plane(const Math::Vector3D& p1, const Math::Vector3D& norm);

		Math::Vector3D normal = { 0.f, 1.f, 0.f }; // unit vector
		float distance = 0.f;  // Distance with origin

		float GetSignedDistanceToPlane(const Math::Vector3D& point) const;
	};
}
