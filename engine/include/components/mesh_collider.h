#pragma once

#include "../physics/collider/collider.h"
#include "../utilities/macro.h"

class ENIGMA_API MeshCollider : public Collider
{
public:
    MeshCollider() = default;
    void Create() override;
    void OnDraw(Math::Vector3D camPos = Math::Vector3D::Zero) override;
    void SetScale() override;
	void Destroy() override;
	
private:
	bool CreateShape();
	RTTR_ENABLE(IComponent)
};
