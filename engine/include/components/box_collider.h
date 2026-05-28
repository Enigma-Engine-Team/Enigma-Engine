#pragma once

#include "../physics/collider/collider.h"
#include "emath/emath.h"
#include "../utilities/macro.h"

class ENIGMA_API BoxCollider : public Collider
{
public:
	BoxCollider() = default;
	BoxCollider(Math::Vector3D scale);
	void Create() override;
	void SetComponent() override;
	void OnDraw(Math::Vector3D camPos = Math::Vector3D::Zero) override;
	void SetScale() override;
	void Destroy() override;

	Math::Vector3D GetScale() const { return scale; }

private:
	JPH::Vec3 ToHalfExtents(const Math::Vector3D& extent);
	void RTTRSetScale(Math::Vector3D _scale);
	RTTR_REGISTRATION_FRIEND
	Math::Vector3D scale;
	RTTR_ENABLE(IComponent)
};