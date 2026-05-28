#include "physics/body.h"

#include "physics/physics.h"
#include "physics/collider/empty_collider.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"

Body::Body(const JPH::BodyCreationSettings& settings)
{
	Physics& myPhysics = Physics::GetInstance();
	bodyID = myPhysics.GetBodyInterface()->CreateAndAddBody(settings, JPH::EActivation::Activate);
}

Body::Body(const JPH::ShapeRefC& shape, const Transform& transform, MotionType mt, Layers layer)
{
	JPH::ShapeRefC shapeSettings = shape;
	Math::Vector3D pos = transform.position;
	Math::Quaternion rot = transform.rotation;
	JPH::Vec3 shapePos(pos.x, pos.y, pos.z);
	JPH::Quat shapeRot(rot.x, rot.y, rot.z, rot.w);

	JPH::BodyCreationSettings settings = {
		shapeSettings,
		shapePos,
		shapeRot,
		static_cast<JPH::EMotionType>(mt),
		static_cast<JPH::ObjectLayer>(layer)
	};

	settings.mAllowDynamicOrKinematic = true;
	settings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
	settings.mMassPropertiesOverride.mMass = 1.0f;
	settings.mAllowSleeping = false;

	Physics& myPhysics = Physics::GetInstance();
	JPH::EActivation activation = mt == MotionType::STATIC ? JPH::EActivation::DontActivate : JPH::EActivation::Activate;
	bodyID = myPhysics.GetBodyInterface()->CreateAndAddBody(settings, activation);
}

void Body::Destroy() const
{
	JPH::BodyInterface* bodyInterface = Physics::GetInstance().GetBodyInterface();
	if (!bodyInterface)
		return;

	bodyInterface->RemoveBody(bodyID);
	bodyInterface->DestroyBody(bodyID);
}

JPH::Body* Body::GetBody() const
{
	return Physics::GetInstance().GetPhysicsSystem()->GetBodyLockInterface().TryGetBody(bodyID);
}