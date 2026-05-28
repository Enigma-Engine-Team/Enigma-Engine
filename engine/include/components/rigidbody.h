#pragma once
#include "../physics/physicalBody.h"
#include "components/icomponent.h"
#include "../utilities/macro.h"
#include "physics/collider/empty_collider.h"

class ENIGMA_API Rigidbody : public IComponent
{
public:
    void Create() override;
    void SetComponent() override;
    void Destroy() override;

    void Activate();
    void Deactivate();

    void SetLinearVelocity(Math::Vector3D& velocity);
    void SetAngularVelocity(Math::Vector3D& velocity);
    void AddForce(Math::Vector3D& force);
    bool GetMotionType();
    void SetMotionType(MotionType mt);
    bool IsTrigger();
    void SetTrigger(bool trigger);
    float GetMass();
    void SetMass(float _mass);
    float GetGravityFactor();
    void SetGravityFactor(float gravity);

private:
    EmptyCollider* emptyCollider = nullptr;
    RTTR_REGISTRATION_FRIEND
    void RTTRSetMotionType(bool kinematic);

    float mass = 0.f;
    RTTR_ENABLE(IComponent)
};
