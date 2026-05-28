#pragma once

#include "body.h"
#include "collision_parameters.h"
#include "physics/collider/collider.h"
#include "../utilities/macro.h"

struct CollisionManager;

class ENIGMA_API Physicalbody
{
public:
    Physicalbody(GameObject* go);
    void Destroy();

    //Physical body handle function
    void AddCollider(Collider* collider);
    void RemoveCollider(Collider* collider);
    float GetNumColliders() const { return static_cast<float>(colliders.size()); }

    bool IsActive() const;
    void Activate();
    void Deactivate();

    //Handle function for Rigidbody
    void SetLinearVelocity(Math::Vector3D& velocity);
    Math::Vector3D GetLinearVelocity();
    void SetAngularVelocity(const Math::Vector3D& velocity);
    Math::Vector3D GetAngularVelocity();
    void AddForce(const Math::Vector3D& force);
    void SetMotionType(MotionType mt);
    MotionType GetMotionType() const { return motionType; }
    bool IsTrigger();
    void SetTrigger(bool trigger);
    float GetMass() const;
    void SetMass(float mass);
    float GetGravityFactor() const;
    void SetGravityFactor(float _gravity) const;

    void UpdateWorldTransform(GameObject* go);
    Transform BuildBodyTransform(GameObject* go);
    //Usual functions in order to update the transform of the gameobject after physics simulation
    //and to update the physics transform after changing the transform of the gameobject
    void UpdateParentTransform(float alpha) const;
    void UpdatePhysicsTransform(bool changedScale);
    void SetPositionAfterPhysics(const Math::Vector3D _positionAfterPhysics);
    void SetRotationAfterPhysics(const Math::Quaternion _rotationAfterPhysics);

    Body* GetBody() const { return body; }
    void AddBody(Body* _body);
    void RemoveBody(const Body* _body);
    void Rebuild();

    CollisionManager* collisionManager;
    GameObject* gameObject;

private:
    void SetActualParameters();
    JPH::ShapeRefC CreateCompound(const std::vector<Collider*> &coll);

    std::vector<Collider*> colliders;

    Math::Vector3D previousPositionAfterPhysics;
    Math::Vector3D positionAfterPhysics;
    Math::Quaternion previousRotationAfterPhysics;
    Math::Quaternion rotationAfterPhysics;
    MotionType motionType;
    Layers layer;
    Body* body;
};
