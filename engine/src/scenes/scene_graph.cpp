#include "scenes/scene_graph.h"

void SceneGraph::UpdateParenting(GameObject* gameObject, Math::Matrix4x4 transformParent)
{
	if(!gameObject)
		return;

	Math::Matrix4x4 local = Math::Quaternion::TRS(gameObject->transform.position, gameObject->transform.rotation, gameObject->transform.scale);
	gameObject->transform.global = transformParent * local;

	gameObject->transform.ExtractPositionFromWorld();
	gameObject->transform.ExtractRotationFromWorld();
	gameObject->transform.ExtractScaleFromWorld();

	for (GameObject* objectChild : gameObject->GetChildren())
	{
		UpdateParenting(objectChild, gameObject->transform.global);
	}

	// if (!gameObject) return;
	// Transform& t = gameObject->transform;
	//
	// if (GameObject* parent = gameObject->GetParent())
	// {
	// 	Transform& p = parent->transform;
	//
	// 	t.worldRotation = p.worldRotation * t.rotation;
	// 	t.worldRotation.Normalize();
	//
	// 	t.worldScale = p.worldScale * t.scale;
	//
	// 	t.worldPosition = t.position;
	// }
	// else
	// {
	// 	t.worldRotation = t.rotation;
	// 	t.worldScale    = t.scale;
	// 	t.worldPosition = t.position;
	// }
	//
	// t.global = Math::Quaternion::TRS(t.worldPosition, t.worldRotation, t.worldScale);
}

void SceneGraph::Update(Scene* scene)
{
	GameObject* firstObject = scene->GetGameObject(0);
	Math::Matrix4x4 identity = Math::Matrix4x4::TRS({ 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f });
	UpdateParenting(firstObject, identity);
}

SceneGraph& SceneGraph::GetInstance()
{
	static SceneGraph instance;
	return instance;
}