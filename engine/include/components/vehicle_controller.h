#pragma once
#include "physics/body.h"
#include "physics/physicalBody.h"
#include "components/icomponent.h"
#include "Jolt/Physics/Vehicle/WheeledVehicleController.h"

class Mesh;

enum DriveType
{
	FWD = 0, // Traction
	RWD = 1, // Propulsion
	AWD = 2  // 4 Roues Motrices
};

class ENIGMA_API VehicleController : public IComponent
{
public:
    VehicleController() = default;
    void Create() override;
    void Destroy() override;
    void SetParent(GameObject* go) override;
	void OnDraw(Math::Vector3D camPos = Math::Vector3D::Zero) override;

	//Scripting
	void SetDriverInput(float inForward, float inRight, float inBrake, float inHandBrake) const;
	float GetRPM() const;
	//Speed in Km/h
	float GetSpeed() const;
	int GetCurrentGear();
	void SetCurrentGear(int currentGear);


	bool IsAutomatic() const { return automatic; }
	void SetTransmissionMode(bool _automatic);

	//Collision
	GameObject* GetCollidingObject() const;
	void SetCollidingObject(GameObject* go);
	void Rebuild();

	//General
	float GetMass() const { return mass; }
	void SetMass(float _mass);
	float GetMaxRollAngle() const { return JPH::RadiansToDegrees(maxRollAngle); }
	void SetMaxRollAngle(float _rollAngle);
	int GetDriveType() const { return driveType; }
	void SetDriveType(int _driveType);

	//Suspensions
	float GetFrontStiffness() const { return frontStiffness; }
	void SetFrontStiffness(float stiffness);
	float GetBackStiffness() const { return backStiffness; }
	void SetBackStiffness(float stiffness);
	float GetFrequency() const { return suspensionFrequency; }
	void SetFrequency(float _frequency);
	float GetDamping() const { return suspensionDamping; }
	void SetDamping(float _damping);

	//Engine
	float GetMaxEngineTorque() const { return maxEngineTorque; }
	void SetMaxEngineToque(float _maxEngineTorque);
	float GetMinRPM() const { return minRPM; }
	void SetMinRPM(float _minRPM);
	float GetMaxRPM() const { return maxRPM; }
	void SetMaxRPM(float _maxRPM);
	float GetInertiaMoment() const { return inertiaMoment; }
	void SetInertiaMoment(float _inertiaMoment);
	float GetAngularDamping() const { return angularDamping; }
	void SetAngularDamping(float _angularDamping);

	//Wheels
    Math::Vector3D GetCenter() const { return center; }
	void SetCenter(Math::Vector3D _center);
	float GetWheelRadius() const { return wheelRadius; }
	void SetWheelRadius(float radius);
	float GetWheelWidth() const { return wheelWidth; }
	void SetWheelWidth(float width);
	float GetHalfVehicleLength() const { return halfVehicleLength; }
	void SetHalfVehicleLength(float length);
	float GetHalfVehicleWidth() const { return halfVehicleWidth; }
	void SetHalfVehicleWidth(float width);
	float GetMaxSteeringAngle() const { return JPH::RadiansToDegrees(maxSteeringAngle); }
	void SetMaxSteeringAngle(float _steerAngle);
	float GetMaxBreakTorque() const { return maxBrakeTorque; }
	void SetMaxBreakTorque(float _maxBreakTorque);

private:
	void CreateBody();
	void CreateCar();
	void DeleteCar();

	//VehiclePart
	void CreateWheels();
	JPH::WheelSettingsWV* CreateFrontWheel(JPH::Vec3 position, JPH::Vec3 flip_x);
	JPH::WheelSettingsWV* CreateRearWheel(JPH::Vec3 position, JPH::Vec3 flip_x);
	void CreateAntiRollbar() const;

	//Controller part
	void SetDifferential() const;
	void SetEngine() const;
	void SetTransmission() const;

	//VehicleConstraints
	void CreateVehicleConstraint();

	//General
	float mass = 600.0f;
	float maxRollAngle = JPH::DegreesToRadians(10.0f);
	bool automatic = true;
	DriveType driveType = RWD;
	int numGears = 7;

	//Raideur dans les virages
	float frontStiffness = 1500.f;
	float backStiffness = 800.f;

	//Suspensions
	float suspensionMinLength = 0.3f;
	float suspensionMaxLength = 0.5f;
	float suspensionFrequency = 6.f;
	float suspensionDamping = 0.7f;

	float frontCasterAngle = 0.0f;
	float frontKingPinAngle = 0.0f;
	float frontCamber = 0.0f;
	float frontToe = 0.0f;
	float frontSuspensionForwardAngle = 0.0f;
	float frontSuspensionSidewaysAngle = 0.0f;

	float rearSuspensionForwardAngle = 0.0f;
	float rearSuspensionSidewaysAngle = 0.0f;
	float rearCasterAngle = 0.0f;
	float rearKingPinAngle = 0.0f;
	float rearCamber = 0.0f;
	float rearToe = 0.0f;

	//Engine
	float maxEngineTorque = 500.0f;
	float minRPM = 1000.f;
	float maxRPM = 7000.f;
	float inertiaMoment = 0.5f;
	float angularDamping = 0.2f;

	//Wheel
	Math::Vector3D center = { 0.f, 0.75f, 0.f };
	float wheelRadius = 0.3f;
	float wheelWidth = 0.2f;
	float halfVehicleLength = 1.4f;
	float halfVehicleWidth = 0.9f;
	float maxBrakeTorque = 4000;
	float maxSteeringAngle = JPH::DegreesToRadians(30.0f);

	Body*									carBody = nullptr;						///< The vehicle
    JPH::Ref<JPH::VehicleConstraint>		vehicleConstraint;						///< The vehicle constraint
    JPH::Ref<JPH::VehicleCollisionTester>	testers[3];							///< Collision testers for the wheels

	// Player input
	JPH::WheeledVehicleController* wheeledController = nullptr;
	JPH::WheeledVehicleControllerSettings* controller = nullptr;
	JPH::VehicleConstraintSettings* vehicle = nullptr;

	GameObject* collidingObject = nullptr;
	RTTR_ENABLE(IComponent)
};
