#include "physics/physics.h"

#include "gameobject/gameobject.h"
#include "scripting/collision_script_manager.h"
#include "Jolt/RegisterTypes.h"
#include "Jolt/Core/Factory.h"

void Physics::Create()
{
	if (JPH::Factory::sInstance)
		return;

	InitializeMultithreading();
	InitializePhysicsSystem();

	bodyInterface = &physicsSystem->GetBodyInterface();
	physicsSystem->OptimizeBroadPhase();
	physicsSystem->SetGravity(JPH::Vec3(0, -9.81f, 0));
}

void Physics::Update(float alpha) const
{
	if (!enable)
		return;
	JPH::BodyIDVector activeBodies;
	physicsSystem->GetActiveBodies(JPH::EBodyType::RigidBody, activeBodies);
	for (JPH::BodyID bodyID : activeBodies)
	{
		uint64_t userData = bodyInterface->GetUserData(bodyID);
		if (userData != 0)
		{
			if (Physicalbody* pb = reinterpret_cast<Physicalbody*>(bodyInterface->GetUserData(bodyID)))
				pb->UpdateParentTransform(alpha);
		}
	}
}

void Physics::FixedUpdate(float deltaTime)
{
	if (!enable)
		return;
	physicsSystem->Update(deltaTime, 1, tempAllocator, jobSystem);
	UpdateActiveBodies();
	UpdateCallBacks();
}

void Physics::Destroy() const
{
	// Unregisters all types with the factory and cleans up the default material
	JPH::UnregisterTypes();

	delete physicsSystem;
	delete jobSystem;
	delete tempAllocator;
	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;
}

void Physics::ClearAllBodies() const
{
	JPH::BodyIDVector allBodies;
	physicsSystem->GetBodies(allBodies);

	if (!allBodies.empty())
	{
		bodyInterface->RemoveBodies(allBodies.data(), static_cast<int>(allBodies.size()));
		bodyInterface->DestroyBodies(allBodies.data(), static_cast<int>(allBodies.size()));
	}

	physicsSystem->OptimizeBroadPhase();
}

void Physics::SetActive(bool enabled)
{
	enable = enabled;
}

Physics& Physics::GetInstance()
{
	static Physics instance;
	return instance;
}

void Physics::UpdateCallBacks()
{
	UpdateContactList(contactsAdded, &CollisionManager::collisionEnter, &Scripting::IScript::OnCollisionEnter);
	UpdateContactList(contactsPersisted, &CollisionManager::collisionStay, &Scripting::IScript::OnCollisionStay);
	UpdateContactList(contactsRemoved, &CollisionManager::collisionExit, &Scripting::IScript::OnCollisionExit);
}

void Physics::UpdateContactList(std::vector<JPH::BodyPair>& contacts, CollisionScriptList scriptList, CollisionCallback callback)
{
	for (int i = static_cast<int>(contacts.size()) - 1; i >= 0; i--)
	{
		JPH::BodyID id1 = contacts[i].mBodyA;
		JPH::BodyID id2 = contacts[i].mBodyB;

		if (id1 == JPH::BodyID())
		{
			contacts.erase(contacts.begin() + i);
			continue;
		}

		JPH::uint64 firstData = bodyInterface->GetUserData(id1);
		JPH::uint64 secData = bodyInterface->GetUserData(id2);

		auto pb = reinterpret_cast<Physicalbody*>(firstData);
		auto pb2 = reinterpret_cast<Physicalbody*>(secData);

		if (!pb || !pb2)
		{
			contacts.erase(contacts.begin() + i);
			continue;
		}

		for (Scripting::IScript* script : pb->collisionManager->*scriptList)
			(script->*callback)(*pb, *pb2);
		for (Scripting::IScript* script : pb2->collisionManager->*scriptList)
			(script->*callback)(*pb2, *pb);

		contacts.erase(contacts.begin() + i);
	}
}

void Physics::UpdateActiveBodies() const
{
	JPH::BodyIDVector activeBodies;
	physicsSystem->GetActiveBodies(JPH::EBodyType::RigidBody, activeBodies);
	for (JPH::BodyID bodyID : activeBodies)
	{
		Physicalbody* pb = reinterpret_cast<Physicalbody*>(bodyInterface->GetUserData(bodyID));

		Math::Vector3D position = {
			bodyInterface->GetPosition(bodyID).GetX(),
			bodyInterface->GetPosition(bodyID).GetY(),
			bodyInterface->GetPosition(bodyID).GetZ()
		};
		Math::Quaternion rotation = {
			bodyInterface->GetRotation(bodyID).GetX(),
			bodyInterface->GetRotation(bodyID).GetY(),
			bodyInterface->GetRotation(bodyID).GetZ(),
			bodyInterface->GetRotation(bodyID).GetW()
		};
		pb->SetPositionAfterPhysics(position);
		pb->SetRotationAfterPhysics(rotation);
	}
}

void Physics::InitializeMultithreading()
{
	JPH::RegisterDefaultAllocator();

	// Required for Jolt initialisation. Without it, Jolt doesn't know how to use his functions
	JPH::Factory::sInstance = new JPH::Factory();
	JPH::RegisterTypes();

	// Temp allocator in order to not over use CPU at start
	tempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);
	// Allow the program to parallelize the actions.
	jobSystem = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);
}

void Physics::InitializePhysicsSystem()
{
	// This is the max amount of rigid bodies that you can add to the physics system.
	const JPH::uint cMaxBodies = 1024;
	// This determines how many mutexes to allocate to protect rigid bodies from concurrent access. Set it to 0 for the default settings.
	const JPH::uint cNumBodyMutexes = 0;

	// This is the max amount of body pairs that can be queued at any time.
	const JPH::uint cMaxBodyPairs = 1024;

	// This is the maximum size of the contact constraint buffer.
	const JPH::uint cMaxContactConstraints = 10240;

	// Now we can create the actual physics system.
	physicsSystem = new JPH::PhysicsSystem();
	physicsSystem->Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, broad_phase_layer_interface, objectVsBroadPhaseLayerFilter, objectVsObjectLayerFilter);
	physicsSystem->SetBodyActivationListener(&bodyActivation_listener);
	physicsSystem->SetContactListener(&contactListener);
}
