#pragma once

#include "components/icomponent.h"
#include "Jolt/Jolt.h"
#include "Jolt/Physics/Collision/Shape/Shape.h"
#include "emath/emath.h"
#include "../../utilities/macro.h"

class ENIGMA_API Collider : public IComponent
{
public:
	void SetParent(GameObject* parent) override;
	void Create() override;
	void Destroy() override;
	virtual void SetScale() {}

	void SetPosition(Math::Vector3D pos);
	void SetRotationFromEuler(Math::Vector3D rot);

	Math::Vector3D GetPosition() const { return position; }
	Math::Quaternion GetRotation() const { return rotation; }
	Math::Vector3D GetEulerRotation() { return rotation.ToEuler(); }
	static Math::Vector3D GetGlobalScale(GameObject* go);
	JPH::ShapeRefC& GetShape() { return shape; }

protected:
	~Collider() override = default;
	JPH::ShapeRefC shape = nullptr;
	Math::Vector3D position{};
	Math::Quaternion rotation{};

private:
	RTTR_ENABLE(IComponent)
};
