#pragma once
#include "emath/emath.h"
#include "../utilities/macro.h"

class GameObject;

struct ENIGMA_API Transform
{
	Transform() = default;
	Transform(Math::Vector3D pos, Math::Quaternion rot, Math::Vector3D scale);
	Transform(Math::Vector3D pos, Math::Vector3D rot, Math::Vector3D scale);
    Math::Vector3D position{ 0.f, 0.f, 0.f };
    Math::Quaternion rotation{ 0.f, 0.f, 0.f, 1.f };
    Math::Vector3D scale{ 1.0f, 1.0f, 1.0f };

    Math::Vector3D worldPosition{};
    Math::Quaternion worldRotation{};
    Math::Vector3D worldScale{ 1.0f, 1.0f, 1.0f };

	void SetPosition(Math::Vector3D pos);
	void SetRotation(Math::Quaternion rot);
	void SetRotation(Math::Vector3D rot);
	void SetScale(Math::Vector3D _scale);

    void ExtractPositionFromWorld();
	void ExtractRotationFromWorld();
	void ExtractScaleFromWorld();

	GameObject* gameObject = nullptr;
    Math::Matrix4x4 global;
};