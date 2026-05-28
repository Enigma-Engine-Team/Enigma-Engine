#pragma once

#include "Jolt/Jolt.h"
#include "Jolt/Core/JobSystemThreadPool.h"
#include "Jolt/Physics/PhysicsSystem.h"
#include "physics/collision_parameters.h"
#include "scripting/collision_script_manager.h"
#include "../utilities/macro.h"

class ENIGMA_API Physics
{
public:
	static Physics& GetInstance();
	void Create();
	void Update(float alpha) const;
	void FixedUpdate(float deltaTime);
	void Destroy() const;
	void ClearAllBodies() const;
	void SetActive(bool enabled);

	JPH::BodyInterface* GetBodyInterface() const { return bodyInterface; }
	JPH::PhysicsSystem* GetPhysicsSystem() const { return physicsSystem; }

	std::vector<JPH::BodyPair> contactsAdded;
	std::vector<JPH::BodyPair> contactsPersisted;
	std::vector<JPH::BodyPair> contactsRemoved;
private:
	using CollisionScriptList = std::vector<Scripting::IScript*> CollisionManager::*;
	using CollisionCallback = void (Scripting::IScript::*)(Physicalbody&, Physicalbody&);

	Physics() = default;
	//Update callbacks for scripts
	void UpdateCallBacks();
	void UpdateContactList(std::vector<JPH::BodyPair>& contacts, CollisionScriptList scriptList, CollisionCallback callback);
	void UpdateActiveBodies() const;
	void InitializeMultithreading();
	void InitializePhysicsSystem();

	BPLayerInterfaceImpl broad_phase_layer_interface;
	ObjectVsBroadPhaseLayerFilterImpl objectVsBroadPhaseLayerFilter;
	ObjectLayerPairFilterImpl objectVsObjectLayerFilter;
	MyBodyActivationListener bodyActivation_listener;
	MyContactListener contactListener;

	JPH::TempAllocatorImpl* tempAllocator = nullptr;
	JPH::JobSystemThreadPool* jobSystem = nullptr;
	JPH::PhysicsSystem* physicsSystem = nullptr;
	JPH::BodyInterface* bodyInterface = nullptr;
	bool enable = true;
};
