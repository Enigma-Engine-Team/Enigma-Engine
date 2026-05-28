#include "physics/collision_parameters.h"
#include "physics/physicalBody.h"
#include "debug/log.h"
#include "scripting/collision_script_manager.h"
#include "physics/physics.h"

bool ObjectLayerPairFilterImpl::ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const
{
	{
		switch (inObject1)
		{
		case static_cast<int>(Layers::NON_MOVING):
			return inObject2 == static_cast<int>(Layers::MOVING); // Non moving only collides with moving
		case static_cast<int>(Layers::MOVING):
			return true; // Moving collides with everything
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
}


bool ObjectVsBroadPhaseLayerFilterImpl::ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const
{
	switch (inLayer1)
	{
	case static_cast<int>(Layers::NON_MOVING):
		return inLayer2 == BroadPhaseLayers::MOVING;
	case static_cast<int>(Layers::MOVING):
		return true;
	default:
		JPH_ASSERT(false);
		return false;
	}
}


BPLayerInterfaceImpl::BPLayerInterfaceImpl()
{
	// Create a mapping table from object to broad phase layer
	mObjectToBroadPhase[static_cast<int>(Layers::NON_MOVING)] = BroadPhaseLayers::NON_MOVING;
	mObjectToBroadPhase[static_cast<int>(Layers::MOVING)] = BroadPhaseLayers::MOVING;
}

JPH::BroadPhaseLayer BPLayerInterfaceImpl::GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const
{
	JPH_ASSERT(inLayer < static_cast<int>(Layers::NUM_LAYERS));
	return mObjectToBroadPhase[inLayer];
}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
const char* BPLayerInterfaceImpl::GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const
{
	switch (static_cast<JPH::BroadPhaseLayer::Type>(inLayer))
	{
	case static_cast<JPH::BroadPhaseLayer::Type>(BroadPhaseLayers::NON_MOVING):
		return "NON_MOVING";
	case static_cast<JPH::BroadPhaseLayer::Type>(BroadPhaseLayers::MOVING):
		return "MOVING";
	default:
		JPH_ASSERT(false);
		return "INVALID";
	}
}
#endif


JPH::ValidateResult MyContactListener::OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult)
{
	inBody1;
	inBody2;
	inBaseOffset;
	inCollisionResult;
	// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
	return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
}

void MyContactListener::OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
{
	inManifold;
	ioSettings;

	auto* pb = reinterpret_cast<Physicalbody*>(inBody1.GetUserData());
	auto* pb2 = reinterpret_cast<Physicalbody*>(inBody2.GetUserData());

	Physics::GetInstance().contactsAdded.emplace_back( pb->GetBody()->GetBodyID(), pb2->GetBody()->GetBodyID() );
}

void MyContactListener::OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
{
	inManifold;
	ioSettings;

	auto* pb = reinterpret_cast<Physicalbody*>(inBody1.GetUserData());
	auto* pb2 = reinterpret_cast<Physicalbody*>(inBody2.GetUserData());

	Physics::GetInstance().contactsPersisted.emplace_back( pb->GetBody()->GetBodyID(), pb2->GetBody()->GetBodyID() );
}

void MyContactListener::OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair)
{
	JPH::BodyID id1 = inSubShapePair.GetBody1ID();
	JPH::BodyID id2 = inSubShapePair.GetBody2ID();

	Physics::GetInstance().contactsRemoved.emplace_back( id1, id2 );
}


void MyBodyActivationListener::OnBodyActivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData)
{
	inBodyID;
	inBodyUserData;
}

void MyBodyActivationListener::OnBodyDeactivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData)
{
	inBodyID;
	inBodyUserData;
}