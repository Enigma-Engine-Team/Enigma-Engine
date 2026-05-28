#include "components/rigidbody.h"
#include "physics/physicalBody.h"
#include "gameobject/gameobject.h"
#include "physics/collider/empty_collider.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<Rigidbody>("Rigidbody")
        .constructor<>()(rttr::policy::ctor::as_raw_ptr)
        .property("Mass", &Rigidbody::GetMass, &Rigidbody::SetMass)
            (rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::DRAG_FLOAT, 0.1f, 1000000.f)))
        .property("Gravity Factor", &Rigidbody::GetGravityFactor, &Rigidbody::SetGravityFactor)
            (rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::DRAG_FLOAT, 0.f, 1000.f)))
        .property("Is Kinematic", &Rigidbody::GetMotionType, &Rigidbody::RTTRSetMotionType)
            (rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::BOOL)))
        .property("Is Trigger", &Rigidbody::IsTrigger, &Rigidbody::SetTrigger)
            (rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::BOOL)));
}

void Rigidbody::Create()
{
    if (!gameObject->physicalBody)
        gameObject->physicalBody = new Physicalbody(gameObject);

    if (gameObject->physicalBody->GetNumColliders() == 0)
    {
        emptyCollider = new EmptyCollider();
        emptyCollider->SetParent(gameObject);
        emptyCollider->Create();
    }
    gameObject->physicalBody->SetMotionType(MotionType::DYNAMIC);
    Activate();
}

void Rigidbody::SetComponent()
{
    SetMass(mass);
}

void Rigidbody::Destroy()
{
    if (gameObject->physicalBody)
    {
        SetMotionType(MotionType::STATIC);
        Deactivate();
        if (emptyCollider)
        {
            gameObject->physicalBody->RemoveCollider(emptyCollider);
            delete emptyCollider;
            emptyCollider = nullptr;
        }
    }
}

void Rigidbody::Activate()
{
    gameObject->physicalBody->Activate();
}

void Rigidbody::Deactivate()
{
    gameObject->physicalBody->Deactivate();
}

void Rigidbody::SetLinearVelocity(Math::Vector3D& velocity)
{
    gameObject->physicalBody->SetLinearVelocity(velocity);
}

void Rigidbody::SetAngularVelocity(Math::Vector3D& velocity)
{
    gameObject->physicalBody->SetAngularVelocity(velocity);
}

void Rigidbody::AddForce(Math::Vector3D& force)
{
    gameObject->physicalBody->AddForce(force);
}

bool Rigidbody::GetMotionType()
{
    if (gameObject->physicalBody->GetMotionType() == MotionType::KINEMATIC)
        return true;
    else
        return false;
}

void Rigidbody::SetMotionType(MotionType mt)
{
    gameObject->physicalBody->SetMotionType(mt);
}

void Rigidbody::RTTRSetMotionType(bool kinematic)
{
    if (kinematic)
        SetMotionType(MotionType::KINEMATIC);
    else
        SetMotionType(MotionType::DYNAMIC);
}

bool Rigidbody::IsTrigger()
{
     return gameObject->physicalBody->IsTrigger();
}

void Rigidbody::SetTrigger(bool trigger)
{
    gameObject->physicalBody->SetTrigger(trigger);
}

float Rigidbody::GetMass()
{
    return gameObject->physicalBody->GetMass();
}

void Rigidbody::SetMass(float _mass)
{
    gameObject->physicalBody->SetMass(_mass);
    mass = _mass;
}

float Rigidbody::GetGravityFactor()
{
    return gameObject->physicalBody->GetGravityFactor();
}

void Rigidbody::SetGravityFactor(float gravity)
{
    gameObject->physicalBody->SetGravityFactor(gravity);
}