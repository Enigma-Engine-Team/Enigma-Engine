#pragma once

#include "../physics/collider/collider.h"
#include "../utilities/macro.h"

class ENIGMA_API SphereCollider : public Collider
{
public:
	SphereCollider() = default;
	void Create() override;
	void OnDraw(Math::Vector3D camPos = Math::Vector3D::Zero) override;
	void SetScale() override;
	void Destroy() override;

	float GetRadius() const { return radius; }
	void SetRadius(float _radius);

private:
	float radius;

	RTTR_ENABLE(IComponent)
};