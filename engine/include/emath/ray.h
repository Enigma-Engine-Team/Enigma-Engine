#pragma once

#include "vector.h"
#include "utility.h"

namespace Math
{
	struct Ray
	{
		Math::Vector3D origin = Math::Vector3D::Zero;
		Math::Vector3D direction = Math::Vector3D::Zero;
		float length = Math::posInfinity;
	};
}