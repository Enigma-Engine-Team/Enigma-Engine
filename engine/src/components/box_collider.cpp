#include "components/box_collider.h"

#include "physics/body.h"
#include "physics/physics.h"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "gameobject/gameobject.h"
#include "Jolt/Renderer/DebugRenderer.h"
#include "renderer/gizmo.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<BoxCollider>("BoxCollider")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("Collider Position", &BoxCollider::GetPosition, &BoxCollider::SetPosition)
		(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::VEC3, 0.f, 1000.f)))
		.property("Collider Rotation", &BoxCollider::GetEulerRotation, &BoxCollider::SetRotationFromEuler)
		(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::VEC3)))
		.property("Collider Scale", &BoxCollider::GetScale, &BoxCollider::RTTRSetScale)
		(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::VEC3, 0.f, 1000.f)));
}

BoxCollider::BoxCollider(Math::Vector3D scale)
{
	this->scale = scale;
	shape = new JPH::BoxShape(ToHalfExtents(scale));
}

void BoxCollider::Create()
{
	scale = { 1.0f, 1.0f, 1.0f };
	shape = new JPH::BoxShape(ToHalfExtents(GetGlobalScale(gameObject) * scale));
	Collider::Create();
}

void BoxCollider::SetComponent()
{
	shape = new JPH::BoxShape(ToHalfExtents(GetGlobalScale(gameObject) * scale));
	gameObject->physicalBody->Rebuild();
}

void BoxCollider::OnDraw(Math::Vector3D camPos)
{
	Gizmos::color = { 0.f, 1.f, 0.f, 1.f };
	Gizmos::DrawCube(
		gameObject->transform.worldPosition + position,
		gameObject->transform.worldRotation * rotation,
		scale * gameObject->transform.worldScale
	);
}

void BoxCollider::SetScale()
{
	if (gameObject->transform.worldScale.x == 0 || gameObject->transform.worldScale.y == 0 || gameObject->transform.worldScale.z == 0 ||
		scale.x == 0 || scale.y == 0 || scale.z == 0)
		return;

	shape = new JPH::BoxShape(ToHalfExtents(GetGlobalScale(gameObject) * scale));
}

void BoxCollider::Destroy()
{
	Collider::Destroy();
}

JPH::Vec3 BoxCollider::ToHalfExtents(const Math::Vector3D& fullScale)
{
	return JPH::Vec3(fullScale.x * 0.5f, fullScale.y * 0.5f, fullScale.z * 0.5f);
}

void BoxCollider::RTTRSetScale(Math::Vector3D _scale)
{
	if (_scale.x <= 0.f)
		_scale.x = 0.001f;
	if (_scale.y <= 0.f)
		_scale.y = 0.001f;
	if (_scale.z <= 0.f)
		_scale.z = 0.001f;
	this->scale = _scale;
	shape = new JPH::BoxShape(ToHalfExtents(GetGlobalScale(gameObject) * scale));
	gameObject->physicalBody->Rebuild();
}
