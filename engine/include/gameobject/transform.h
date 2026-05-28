#pragma once
#include "emath/emath.h"
#include "../utilities/macro.h"
#include "rttr/registration_friend.h"

class GameObject;

class ENIGMA_API Transform
{
public:
	Transform() = default;

	Transform(Math::Vector3D pos, Math::Quaternion rot, Math::Vector3D scale);

	Transform(Math::Vector3D pos, Math::Vector3D rot, Math::Vector3D scale);

	void SetPosition(Math::Vector3D pos);
	void SetRotation(Math::Quaternion rot);
	void SetRotation(Math::Vector3D rot);
	void SetScale(Math::Vector3D _scale);

	Math::Vector3D GetPosition() const { return position; }
	Math::Vector3D GetScale() const { return scale; }
	Math::Quaternion GetRotation() const { return rotation; }

	Math::Vector3D GetWorldPosition() const { return worldPosition; }
	Math::Vector3D GetWorldScale() const { return worldScale; }
	Math::Quaternion GetWorldRotation() const { return worldRotation; }

    void ExtractPositionFromWorld();
	void ExtractRotationFromWorld();
	void ExtractScaleFromWorld();

	Math::Matrix4x4 GetGlobalMatrix() const { return global; };

private:

	RTTR_REGISTRATION_FRIEND
	Math::Vector3D position{ 0.f, 0.f, 0.f };
	Math::Quaternion rotation{ 0.f, 0.f, 0.f, 1.f };
	Math::Vector3D scale{ 1.0f, 1.0f, 1.0f };

	Math::Vector3D worldPosition{};
	Math::Quaternion worldRotation{};
	Math::Vector3D worldScale{ 1.0f, 1.0f, 1.0f };

	GameObject* gameObject = nullptr;
    Math::Matrix4x4 global;

	friend class GameObject;
	friend class SceneGraph;
};