#include "gameobject/transform.h"
#include "scenes/scene_graph.h"
#include "gameobject/gameobject.h"
#include "scenes/scene_manager.h"
#include "utilities/shortcut.h"
#include "window/input_manager.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<Transform>("Transform")
		 .property("position", &Transform::GetPosition, &Transform::SetPosition)
		 .property("rotation", &Transform::GetRotation, &Transform::SetRotation)
		 .property("scale", &Transform::GetScale, &Transform::SetScale);
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
	SetRotationToEuler(rot);
	SetScale(scale);
}

void Transform::SetPosition(Math::Vector3D pos)
{
	if (InputManager::GetMouseButtonClicked(MOUSE_BUTTON_LEFT) ||
	   (InputManager::GetKeyDown(KEY_LEFT_CONTROL) && (InputManager::GetKeyDown(KEY_Z) || InputManager::GetKeyDown(KEY_Y))))
	{
		Math::Vector3D oldPos = position;
		Action action("SetPosition", {}, gameObject->GetUUID());
		action.action = [action, oldPos]() {
			GameObject* go = SceneManager::GetInstance().GetCurrentScene()->GetGameObject(action.goID);
			go->transform.SetPosition(oldPos);
		};
		Shortcut::AddAction(action);
	}

	position = pos;

	if (gameObject)
	{
		if (gameObject->GetParent())
			SceneGraph::GetInstance().UpdateParenting(gameObject, gameObject->GetParent()->transform.global);

		if (Physicalbody* pb = gameObject->physicalBody)
			pb->UpdatePhysicsTransform(false);
	}
}

void Transform::SetRotation(const Math::Quaternion rot)
{
	if (InputManager::GetMouseButtonClicked(MOUSE_BUTTON_LEFT) ||
	   (InputManager::GetKeyDown(KEY_LEFT_CONTROL) && (InputManager::GetKeyDown(KEY_Z) || InputManager::GetKeyDown(KEY_Y))))
	{
		Math::Quaternion oldRot = rotation;
		Action action("SetRotationToEuler", {}, gameObject->GetUUID());
		action.action = [action, oldRot]() {
			GameObject* go = SceneManager::GetInstance().GetCurrentScene()->GetGameObject(action.goID);
			go->transform.SetRotation(oldRot);
		};
		Shortcut::AddAction(action);
	}

	rotation = rot;

	if (gameObject)
	{
		if (gameObject->GetParent())
			SceneGraph::GetInstance().UpdateParenting(gameObject, gameObject->GetParent()->transform.global);

		if (Physicalbody* pb = gameObject->physicalBody)
			pb->UpdatePhysicsTransform(false);
	}
}

void Transform::SetRotationToEuler(Math::Vector3D rot)
{
	if (InputManager::GetMouseButtonClicked(MOUSE_BUTTON_LEFT) ||
	   (InputManager::GetKeyDown(KEY_LEFT_CONTROL) && (InputManager::GetKeyDown(KEY_Z) || InputManager::GetKeyDown(KEY_Y))))
	{
		Math::Quaternion oldRot = rotation;
		Action action("SetRotationToEuler", {}, gameObject->GetUUID());
		action.action = [action, oldRot]() {
			GameObject* go = SceneManager::GetInstance().GetCurrentScene()->GetGameObject(action.goID);
			go->transform.SetRotation(oldRot);
		};
		Shortcut::AddAction(action);
	}

	Math::Quaternion rotQuat = Math::Quaternion::FromEuler(rot);
	SetRotation(rotQuat);
}

void Transform::SetScale(Math::Vector3D _scale)
{
	if (InputManager::GetMouseButtonClicked(MOUSE_BUTTON_LEFT) ||
	   (InputManager::GetKeyDown(KEY_LEFT_CONTROL) && (InputManager::GetKeyDown(KEY_Z) || InputManager::GetKeyDown(KEY_Y))))
	{
		Math::Vector3D oldScale = scale;
		Action action("SetScale", {}, gameObject->GetUUID());
		action.action = [action, oldScale]() {
			GameObject* go = SceneManager::GetInstance().GetCurrentScene()->GetGameObject(action.goID);
			go->transform.SetScale(oldScale);
		};
		Shortcut::AddAction(action);
	}

	scale = _scale;

	if (gameObject)
	{
		if (gameObject->GetParent())
			SceneGraph::GetInstance().UpdateParenting(gameObject, gameObject->GetParent()->transform.global);

		if (Physicalbody* pb = gameObject->physicalBody)
			pb->UpdatePhysicsTransform(true);
	}
}

void Transform::ExtractPositionFromWorld()
{
	worldPosition = { global.m[12], global.m[13], global.m[14] };
}

void Transform::ExtractRotationFromWorld()
{
	if (!gameObject->GetParent())
	{
		worldRotation = rotation;
		return;
	}
	gameObject->transform.worldRotation = gameObject->GetParent()->transform.worldRotation * gameObject->transform.rotation;
	gameObject->transform.worldRotation.Normalize();
}

void Transform::ExtractScaleFromWorld()
{
	Math::Vector3D col0 = { global.m[0], global.m[1], global.m[2] };
	Math::Vector3D col1 = { global.m[4], global.m[5], global.m[6] };
	Math::Vector3D col2 = { global.m[8], global.m[9], global.m[10] };

	worldScale = {col0.Magnitude(), col1.Magnitude(), col2.Magnitude()};
}