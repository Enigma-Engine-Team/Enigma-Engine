#include "components/sphere_collider.h"

#include "physics/body.h"
#include "physics/physics.h"
#include "gameobject/gameobject.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "renderer/gizmo.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<SphereCollider>("SphereCollider")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("Collider Center", &SphereCollider::GetPosition, &SphereCollider::SetPosition)
		(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::VEC3, 0.f, 1000.f)))
		.property("Collider Rotation", &SphereCollider::GetEulerRotation, &SphereCollider::SetRotationFromEuler)
		(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::VEC3)))
		.property("Collider Radius", &SphereCollider::GetRadius, &SphereCollider::SetRadius)
		(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::DRAG_FLOAT, 0.001f, 1000.f, "%.3f")));
}

void SphereCollider::Create()
{
	radius = 0.5f;
	shape = new JPH::SphereShape(radius * GetGlobalScale(gameObject).x);
	Collider::Create();
}

void SphereCollider::OnDraw(Math::Vector3D camPos)
{
	Gizmos::color = { 0.f, 1.f, 0.f, 1.f };
	Gizmos::DrawSphere(gameObject->transform.worldPosition, radius * gameObject->transform.worldScale.x);
}

void SphereCollider::SetScale()
{
	if (gameObject->transform.worldScale.x == 0 || radius == 0)
		return;

	shape = new JPH::SphereShape(radius * GetGlobalScale(gameObject).x);
}

void SphereCollider::Destroy()
{
	Collider::Destroy();
}

void SphereCollider::SetRadius(float _radius)
{
	if (_radius <= 0.f)
		_radius = 0.001f;

	this->radius = _radius;
	shape = new JPH::SphereShape(radius * GetGlobalScale(gameObject).x);
	gameObject->physicalBody->Rebuild();
}
