#include "physics/collider/empty_collider.h"

#include "physics/body.h"
#include "Jolt/Physics/Collision/Shape/EmptyShape.h"

EmptyCollider::EmptyCollider()
{
	shape = new JPH::EmptyShape();
}

void EmptyCollider::Create()
{
	Collider::Create();
}
