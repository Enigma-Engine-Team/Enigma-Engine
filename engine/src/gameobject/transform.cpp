#include "gameobject/transform.h"
#include "scenes/scene_graph.h"
#include "gameobject/gameobject.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<Transform>("Transform")
		 .property("position", &Transform::position)
		 .property("rotation", &Transform::rotation)
		 .property("scale", &Transform::scale);
}

Transform::Transform(Math::Vector3D pos, Math::Quaternion rot, Math::Vector3D scale)
{
	SetPosition(pos);
	SetRotation(rot);
	SetScale(scale);
}

Transform::Transform(Math::Vector3D pos, Math::Vector3D rot, Math::Vector3D scale)
{
	SetPosition(pos);
	SetRotation(rot);
	SetScale(scale);
}

void Transform::SetPosition(Math::Vector3D pos)
{
	position = pos;

	if (gameObject)
	{
		if (gameObject->GetParent())
			SceneGraph::GetInstance().UpdateParenting(gameObject, gameObject->GetParent()->transform.global);

		if (Physicalbody* pb = gameObject->physicalBody)
			pb->UpdatePhysicsTransform(false);
	}
}

void Transform::SetRotation(Math::Quaternion rot)
{
	rotation = rot;

	if (gameObject)
	{
		if (gameObject->GetParent())
			SceneGraph::GetInstance().UpdateParenting(gameObject, gameObject->GetParent()->transform.global);

		if (Physicalbody* pb = gameObject->physicalBody)
			pb->UpdatePhysicsTransform(false);
	}
}

void Transform::SetRotation(Math::Vector3D rot)
{
	Math::Quaternion rotQuat = Math::Quaternion::FromEuler(rot);
	SetRotation(rotQuat);
}

void Transform::SetScale(Math::Vector3D _scale)
{
	scale = _scale;

	if (gameObject)
	{
		if (gameObject->GetParent())
			SceneGraph::GetInstance().UpdateParenting(gameObject, gameObject->GetParent()->transform.global);

		if (Physicalbody* pb = gameObject->physicalBody)
			pb->UpdatePhysicsTransform(false);
	}
}

void Transform::ExtractPositionFromWorld()
{
	worldPosition = { global.m[12], global.m[13], global.m[14] };
}

void Transform::ExtractRotationFromWorld()
{
    Math::Matrix3x3 rotMat;
    rotMat.m[0] = global.m[0] / worldScale.x;
    rotMat.m[1] = global.m[1] / worldScale.x;
    rotMat.m[2] = global.m[2] / worldScale.x;

    rotMat.m[3] = global.m[4] / worldScale.y;
    rotMat.m[4] = global.m[5] / worldScale.y;
    rotMat.m[5] = global.m[6] / worldScale.y;

    rotMat.m[6] = global.m[8] / worldScale.z;
    rotMat.m[7] = global.m[9] / worldScale.z;
    rotMat.m[8] = global.m[10] / worldScale.z;

    worldRotation = Math::Quaternion::FromRotationMatrix(rotMat);
}

void Transform::ExtractScaleFromWorld()
{
	Math::Vector3D col0 = { global.m[0], global.m[1], global.m[2] };
	Math::Vector3D col1 = { global.m[4], global.m[5], global.m[6] };
	Math::Vector3D col2 = { global.m[8], global.m[9], global.m[10] };

	worldScale = {col0.Magnitude(), col1.Magnitude(), col2.Magnitude()};
}