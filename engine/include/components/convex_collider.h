#pragma once
#include "../physics/collider/collider.h"
#include "resources/mesh.h"

class ConvexCollider : public Collider
{
public:
    ConvexCollider() = default;
    void Create() override;
    void OnDraw(Math::Vector3D camPos = Math::Vector3D::Zero) override;
    void SetScale() override;
    static JPH::ShapeRefC CreateShape(GameObject* go);
    static JPH::ShapeRefC CreateShape(GameObject* go, std::vector<Mesh*>& meshes);
private:
    RTTR_ENABLE(IComponent);
};