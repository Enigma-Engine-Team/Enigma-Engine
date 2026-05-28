#pragma once

#include "plane.h"

namespace Math
{
	struct Frustum
	{
		Plane topFace;
		Plane bottomFace;

		Plane rightFace;
		Plane leftFace;

		Plane farFace;
		Plane nearFace;
	};
}
