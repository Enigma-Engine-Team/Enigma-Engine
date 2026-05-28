#include "emath/plane.h"

Math::Plane::Plane(const Math::Vector3D& p1, const Math::Vector3D& norm)
{
	normal = norm.Normalized();
	distance = normal.DotProduct(p1);
}

float Math::Plane::GetSignedDistanceToPlane(const Math::Vector3D& point) const
{
	return normal.DotProduct(point) - distance;
}
