#pragma once

#include "physics/collider/collider.h"
#include "collision_parameters.h"
#include "gameobject/transform.h"
#include "Jolt/Physics/Body/Body.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"

class Body
{
public:
	Body(const JPH::BodyCreationSettings& settings);
	Body(const JPH::ShapeRefC &shape, const Transform &transform, MotionType mt = MotionType::STATIC, Layers layer = Layers::NON_MOVING);
	void Destroy() const;

	JPH::Body* GetBody() const;
	JPH::BodyID GetBodyID() const { return bodyID; }
private:
	JPH::BodyID bodyID;
};
