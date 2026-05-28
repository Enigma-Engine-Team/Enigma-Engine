#pragma once

#include "emath/vector.h"

namespace Math
{
	struct Triangle
	{
		Math::Vector3D a = Math::Vector3D::Zero;
		Math::Vector3D b = Math::Vector3D::Zero;
		Math::Vector3D c = Math::Vector3D::Zero;
	};
}