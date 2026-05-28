#include "physics/physicalBody.h"

#include "physics/physics.h"
#include "gameobject/gameobject.h"

#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Collision/Shape/StaticCompoundShape.h"
#include "components/mesh_collider.h"
#include "scenes/scene_graph.h"
#include "scripting/collision_script_manager.h"

Physicalbody::Physicalbody(GameObject* go)
{
    gameObject = go;
    collisionManager = new CollisionManager();
    previousPositionAfterPhysics = gameObject->transform.position;
    positionAfterPhysics = gameObject->transform.position;
    previousRotationAfterPhysics = gameObject->transform.rotation;
    rotationAfterPhysics = gameObject->transform.rotation;
    motionType = MotionType::STATIC;
    layer = Layers::NON_MOVING;
    body = nullptr;
}

void Physicalbody::Destroy()
{
    if (body)
    {
        Physics::GetInstance().GetBodyInterface()->SetUserData(body->GetBodyID(), 0);
        Physics::GetInstance().GetBodyInterface()->RemoveBody(body->GetBodyID());
        Physics::GetInstance().GetBodyInterface()->DestroyBody(body->GetBodyID());
        delete body;
        body = nullptr;
    }
}

void Physicalbody::AddCollider(Collider* collider)
{
    if (std::find(colliders.begin(), colliders.end(), collider) == colliders.end())
        colliders.push_back(collider);
    Rebuild();
}

void Physicalbody::RemoveCollider(Collider *collider)
{
    auto it = std::find(colliders.begin(), colliders.end(), collider);
    if (it != colliders.end())
    {
        colliders.erase(it);
        if (colliders.empty())
            Destroy();
        Rebuild();
    }
}

bool Physicalbody::IsActive() const
{
    if (!body)
        return false;

    return Physics::GetInstance().GetBodyInterface()->IsActive(body->GetBodyID());
}

void Physicalbody::Activate()
{
    if (!body)
        return;

    Physics::GetInstance().GetBodyInterface()->ActivateBody(body->GetBodyID());
}

void Physicalbody::Deactivate()
{
    if (!body)
        return;

    Physics::GetInstance().GetBodyInterface()->DeactivateBody(body->GetBodyID());
}



void Physicalbody::SetLinearVelocity(Math::Vector3D& velocity)
{
    if (!body)
        return;

    JPH::Vec3 vel(velocity.x, velocity.y, velocity.z);
    Physics::GetInstance().GetBodyInterface()->SetLinearVelocity(body->GetBodyID(), vel);
    if (velocity != Math::Vector3D::Zero)
    {
        SetMotionType(MotionType::DYNAMIC);
        Activate();
    }
}

Math::Vector3D Physicalbody::GetLinearVelocity()
{
    JPH::Vec3 vel = Physics::GetInstance().GetBodyInterface()->GetLinearVelocity(body->GetBodyID());
    return {vel.GetX(), vel.GetY(), vel.GetZ()};
}

void Physicalbody::SetAngularVelocity(const Math::Vector3D& velocity)
{
    if (!body)
        return;

    JPH::Vec3 vel(velocity.x, velocity.y, velocity.z);
    Physics::GetInstance().GetBodyInterface()->SetAngularVelocity(body->GetBodyID(), vel);
    if (velocity != Math::Vector3D::Zero)
    {
        SetMotionType(MotionType::DYNAMIC);
        Activate();
    }
}

Math::Vector3D Physicalbody::GetAngularVelocity()
{
    JPH::Vec3 vel = Physics::GetInstance().GetBodyInterface()->GetAngularVelocity(body->GetBodyID());
    return {vel.GetX(), vel.GetY(), vel.GetZ()};
}

void Physicalbody::AddForce(const Math::Vector3D& force)
{
    if (!body)
        return;

    JPH::Vec3 f(force.x, force.y, force.z);
    Physics::GetInstance().GetBodyInterface()->AddForce(body->GetBodyID(), f);
    if (force != Math::Vector3D::Zero)
    {
        SetMotionType(MotionType::DYNAMIC);
        Activate();
    }
}

void Physicalbody::SetMotionType(MotionType mt)
{
    motionType = mt;
    layer = (mt == MotionType::STATIC) ? Layers::NON_MOVING : Layers::MOVING;
    if (!body) return;

    JPH::BodyInterface* bi = Physics::GetInstance().GetBodyInterface();
    JPH::EActivation act = (mt == MotionType::STATIC) ? JPH::EActivation::DontActivate : JPH::EActivation::Activate;

    bi->SetObjectLayer(body->GetBodyID(), static_cast<JPH::ObjectLayer>(layer));
    bi->SetMotionType(body->GetBodyID(), static_cast<JPH::EMotionType>(mt), act);
}

bool Physicalbody::IsTrigger()
{
    JPH::BodyInterface* bi = Physics::GetInstance().GetBodyInterface();
    return bi->IsSensor(body->GetBodyID());
}

void Physicalbody::SetTrigger(bool trigger)
{
    if (!body)
        return;
    JPH::BodyInterface* bi = Physics::GetInstance().GetBodyInterface();
    bi->RemoveBody(body->GetBodyID());
    bi->SetIsSensor(body->GetBodyID(), trigger);
    bi->AddBody(body->GetBodyID(), JPH::EActivation::Activate);
}

float Physicalbody::GetMass() const
{
    if (motionType == MotionType::STATIC || motionType == MotionType::KINEMATIC)
        return 0.f;

    return 1.0f / body->GetBody()->GetMotionProperties()->GetInverseMass();
}

void Physicalbody::SetMass(float mass)
{
    if (motionType == MotionType::STATIC || motionType == MotionType::KINEMATIC)
        return;

    if (mass <= 0.f)
        mass = 0.001f;
    JPH::MotionProperties* mp = body->GetBody()->GetMotionProperties();
    mp->ScaleToMass(mass);
}

float Physicalbody::GetGravityFactor() const
{
    return Physics::GetInstance().GetBodyInterface()->GetGravityFactor(body->GetBodyID());
}

void Physicalbody::SetGravityFactor(float _gravity) const
{
    JPH::BodyInterface* bi = Physics::GetInstance().GetBodyInterface();
    bi->SetGravityFactor(body->GetBodyID(), _gravity);
}



void Physicalbody::UpdateWorldTransform(GameObject* go)
{
    if (GameObject* parent = go->GetParent())
    {
        UpdateWorldTransform(parent);
        SceneGraph::GetInstance().UpdateParenting(go, parent->transform.global);
    }
    else
        SceneGraph::GetInstance().UpdateParenting(go, Math::Matrix4x4::TRS({ 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f }));
}

Transform Physicalbody::BuildBodyTransform(GameObject* go)
{
    UpdateWorldTransform(go);

    Transform bodyTransform;
    bodyTransform.position = go->transform.worldPosition;
    bodyTransform.rotation = go->transform.worldRotation;
    bodyTransform.scale = go->transform.worldScale;
    return bodyTransform;
}

void Physicalbody::UpdateParentTransform(float alpha) const
{
    if (alpha < 0.0f)
        alpha = 0.0f;
    else if (alpha > 1.0f)
        alpha = 1.0f;

    gameObject->transform.position = Math::Vector3D::Lerp(previousPositionAfterPhysics, positionAfterPhysics, alpha);
    gameObject->transform.rotation = Math::Quaternion::Slerp(previousRotationAfterPhysics, rotationAfterPhysics, alpha);
}

void Physicalbody::UpdatePhysicsTransform(bool changedScale)
{
    if (!body)
        return;

    if (changedScale)
    {
        for (Collider* collider : colliders)
        {
            collider->SetScale();
        }
    }
    Rebuild();

    Math::Vector3D newPos = gameObject->transform.worldPosition;
    Math::Quaternion newRot = (gameObject->transform.worldRotation).Conjugate();

    Physics::GetInstance().GetBodyInterface()->SetPositionAndRotation(
        body->GetBodyID(),
        JPH::Vec3(newPos.x, newPos.y, newPos.z),
        JPH::Quat(newRot.x, newRot.y, newRot.z, newRot.w),
        JPH::EActivation::Activate
    );

    SetPositionAfterPhysics(newPos);
    SetRotationAfterPhysics(newRot);

    if (motionType == MotionType::STATIC)
        return;
    body->GetBody()->SetLinearVelocity(JPH::Vec3::sZero());
    body->GetBody()->SetAngularVelocity(JPH::Vec3::sZero());
}

void Physicalbody::SetPositionAfterPhysics(const Math::Vector3D _positionAfterPhysics)
{
    previousPositionAfterPhysics = positionAfterPhysics;
    positionAfterPhysics = _positionAfterPhysics;
}

void Physicalbody::SetRotationAfterPhysics(const Math::Quaternion _rotationAfterPhysics)
{
    previousRotationAfterPhysics = rotationAfterPhysics;
    rotationAfterPhysics = _rotationAfterPhysics;
}



void Physicalbody::AddBody(Body* _body)
{
    body = _body;
    Rebuild();
}

void Physicalbody::RemoveBody(const Body* _body)
{
    if (body == _body)
    {
        Destroy();
    }
}

void Physicalbody::Rebuild()
{
    if (colliders.empty() && body)
    {
        Physics::GetInstance().GetBodyInterface()->SetUserData(body->GetBodyID(), reinterpret_cast<JPH::uint64>(this));
    }
    else if (colliders.empty() && !body)
    {
        Destroy();
        gameObject->physicalBody = nullptr;
        delete this;
        return;
    }

    if (colliders.empty())
        return;

    bool updateMass = true;
    if (dynamic_cast<MeshCollider*>(colliders[0]))
        updateMass = false;

    const JPH::ShapeRefC finalShape = CreateCompound(colliders);

    if (!body)
    {
        body = new Body(finalShape, BuildBodyTransform(gameObject), motionType, layer);
        Physics::GetInstance().GetBodyInterface()->SetUserData(body->GetBodyID(), reinterpret_cast<JPH::uint64>(this));
    }
    else
    {
        Physics::GetInstance().GetBodyInterface()->SetShape(
            body->GetBodyID(),
            finalShape,
            updateMass,
            JPH::EActivation::Activate
        );
    }

    SetActualParameters();
    if (motionType != MotionType::STATIC)
        Activate();
}

void Physicalbody::SetActualParameters()
{
    SetMotionType(motionType);
}

JPH::ShapeRefC Physicalbody::CreateCompound(const std::vector<Collider*>& coll)
{
    JPH::StaticCompoundShapeSettings compoundSettings;

    for (Collider* collider : coll)
    {
        Math::Quaternion rot = collider->GetRotation().Conjugate();

        JPH::Vec3 position(collider->GetPosition().x, collider->GetPosition().y, collider->GetPosition().z);
        JPH::Quat rotation(rot.x, rot.y, rot.z, rot.w);
        compoundSettings.AddShape(position, rotation, collider->GetShape());
    }

    JPH::ShapeSettings::ShapeResult result = compoundSettings.Create();

    return result.Get();
}
