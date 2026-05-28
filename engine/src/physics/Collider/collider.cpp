#include "physics/collider/collider.h"
#include "gameobject/gameobject.h"

void Collider::SetParent(GameObject *_parent)
{
	IComponent::SetParent(_parent);
	position = Math::Vector3D::Zero;
	rotation = Math::Quaternion::Identity;
}

void Collider::Create()
{
	if (!gameObject->physicalBody)
		gameObject->physicalBody = new Physicalbody(gameObject);

	gameObject->physicalBody->AddCollider(this);
}

void Collider::Destroy()
{
	if (Physicalbody* pb = gameObject->physicalBody)
		pb->RemoveCollider(this);
}

void Collider::SetPosition(Math::Vector3D pos)
{
	position = pos;
	if (gameObject->physicalBody)
		gameObject->physicalBody->UpdatePhysicsTransform(false);
}

void Collider::SetRotationFromEuler(Math::Vector3D rot)
{
	rotation = Math::Quaternion::FromEuler(rot).Conjugate();
	if (gameObject->physicalBody)
		gameObject->physicalBody->UpdatePhysicsTransform(false);
}

Math::Vector3D Collider::GetGlobalScale(GameObject* go)
{
	Math::Vector3D scale = Math::Vector3D::One;
	while(go)
	{
		scale *= go->transform.scale;
		go = go->GetParent();
	}
	return scale;
}
