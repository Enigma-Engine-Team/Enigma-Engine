#include "emath/aabb.h"
#include <rttr/registration.h>
#include <iostream>
#include <algorithm>

RTTR_REGISTRATION
{
	rttr::registration::class_<Math::AABB>("AABB")
		 .property("minCorner", &Math::AABB::minCorner)
		 .property("maxCorner", &Math::AABB::maxCorner);
}

void Math::AABB::Update(Math::Matrix4x4 parentTRS)
{
	p1 = parentTRS * Math::Vector4D(minCorner.x, minCorner.y, minCorner.z, 1.f);
	p2 = parentTRS * Math::Vector4D(maxCorner.x, minCorner.y, minCorner.z, 1.f);
	p3 = parentTRS * Math::Vector4D(maxCorner.x, minCorner.y, maxCorner.z, 1.f);
	p4 = parentTRS * Math::Vector4D(minCorner.x, minCorner.y, maxCorner.z, 1.f);

	p5 = parentTRS * Math::Vector4D(minCorner.x, maxCorner.y, minCorner.z, 1.f);
	p6 = parentTRS * Math::Vector4D(maxCorner.x, maxCorner.y, minCorner.z, 1.f);
	p7 = parentTRS * Math::Vector4D(maxCorner.x, maxCorner.y, maxCorner.z, 1.f);
	p8 = parentTRS * Math::Vector4D(minCorner.x, maxCorner.y, maxCorner.z, 1.f);

	xMin = std::min({ p1.x, p2.x, p3.x, p4.x, p5.x, p6.x, p7.x, p8.x });
	xMax = std::max({ p1.x, p2.x, p3.x, p4.x, p5.x, p6.x, p7.x, p8.x });
	
	yMin = std::min({ p1.y, p2.y, p3.y, p4.y, p5.y, p6.y, p7.y, p8.y });
	yMax = std::max({ p1.y, p2.y, p3.y, p4.y, p5.y, p6.y, p7.y, p8.y });
	
	zMin = std::min({ p1.z, p2.z, p3.z, p4.z, p5.z, p6.z, p7.z, p8.z });
	zMax = std::max({ p1.z, p2.z, p3.z, p4.z, p5.z, p6.z, p7.z, p8.z });
}
