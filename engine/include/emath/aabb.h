#pragma once

#include "vector.h"
#include "matrix.h"

namespace Math
{
	struct AABB
	{
		Math::Vector3D minCorner = Math::Vector3D::Zero;
		Math::Vector3D maxCorner = Math::Vector3D::Zero;

		Math::Vector3D p1 = Math::Vector3D::Zero;
		Math::Vector3D p2 = Math::Vector3D::Zero;
		Math::Vector3D p3 = Math::Vector3D::Zero;
		Math::Vector3D p4 = Math::Vector3D::Zero;
		Math::Vector3D p5 = Math::Vector3D::Zero;
		Math::Vector3D p6 = Math::Vector3D::Zero;
		Math::Vector3D p7 = Math::Vector3D::Zero;
		Math::Vector3D p8 = Math::Vector3D::Zero;

		float xMin = 0.f;
		float xMax = 0.f;

		float yMin = 0.f;
		float yMax = 0.f;

		float zMin = 0.f;
		float zMax = 0.f;

		void Update(Math::Matrix4x4 parentTRS);
	};
}