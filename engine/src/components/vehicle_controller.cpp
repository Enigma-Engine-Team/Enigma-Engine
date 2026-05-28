#include "components/vehicle_controller.h"

#include "components/convex_collider.h"
#include "physics/collision_parameters.h"
#include "physics/physics.h"
#include "gameobject/gameobject.h"
#include "Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.h"
#include "Jolt/Physics/Vehicle/VehicleController.h"
#include "renderer/gizmo.h"
#include "components/mesh_renderer.h"
#include "physics/collider/empty_collider.h"
#include "scenes/scene_manager.h"

RTTR_REGISTRATION{
	rttr::registration::class_<VehicleController>("Vehicle Controller")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("Collider", &VehicleController::GetCollidingObject, &VehicleController::SetCollidingObject)
			(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::GAMEOBJECT_INPUT, 0.1f, 10000.f, "%.1f", "Collider")))
		.property("Mass", &VehicleController::GetMass, &VehicleController::SetMass)
			(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::DRAG_FLOAT, 0.1f, 10000.f, "%.1f", "General")))
		.property("Roll Angle", &VehicleController::GetMaxRollAngle, &VehicleController::SetMaxRollAngle)
			(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::SLIDER_FLOAT, 0.1f, 90.f, "%.1f", "General")))
		.property("Drive Type", &VehicleController::GetDriveType, &VehicleController::SetDriveType)
			(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::COMBO, 0.1f, 1.f, "%.1f", "General", {"FWD", "RWD", "AWD"})))

		.property("Max Engine Torque", &VehicleController::GetMaxEngineTorque, &VehicleController::SetMaxEngineToque)
			(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::DRAG_FLOAT, 0.f, 20000.f, "%.1f", "Engine")))
		.property("Min RPM", &VehicleController::GetMinRPM, &VehicleController::SetMinRPM)
			(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::DRAG_FLOAT, 0.f, 3000.f, "%.1f", "Engine")))
		.property("Max RPM", &VehicleController::GetMaxRPM, &VehicleController::SetMaxRPM)
			(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::DRAG_FLOAT, 0.f, 20000.f, "%.1f", "Engine")))
		.property("Inertia Moment", &VehicleController::GetInertiaMoment, &VehicleController::SetInertiaMoment)
			(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::DRAG_FLOAT, 0.f, 5.f, "%.3f", "Engine")))
		.property("Angular Damping", &VehicleController::GetAngularDamping, &VehicleController::SetAngularDamping)
			(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::DRAG_FLOAT, 0.f, 1.f, "%.3f", "Engine")))

		.property("Front Stiffness", &VehicleController::GetFrontStiffness, &VehicleController::SetFrontStiffness)
			(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::DRAG_FLOAT, 100.f, 10000.f, "%.0f", "Suspensions")))
		.property("Back Stiffness", &VehicleController::GetBackStiffness, &VehicleController::SetBackStiffness)
			(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::DRAG_FLOAT, 100.f, 10000.f, "%.0f", "Suspensions")))
		.property("Frequency", &VehicleController::GetFrequency, &VehicleController::SetFrequency)
			(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::DRAG_FLOAT, 1.f, 10.f, "%.1f", "Suspensions")))
		.property("Damping", &VehicleController::GetDamping, &VehicleController::SetDamping)
			(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::DRAG_FLOAT, 0.f, 1.f, "%.3f", "Suspensions")))

		.property("Automatic", &VehicleController::IsAutomatic, &VehicleController::SetTransmissionMode)
			(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::BOOL, 100.f, 4000.f, "%.0f", "Transmission")))

		.property("halfVehicleWidth", &VehicleController::GetHalfVehicleWidth, &VehicleController::SetHalfVehicleWidth)
			(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::DRAG_FLOAT, 0.f, 10.f, "%.1f", "Wheel")))
		.property("halfVehicle_Length", &VehicleController::GetHalfVehicleLength, &VehicleController::SetHalfVehicleLength)
			(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::DRAG_FLOAT, 0.f, 10.f, "%.1f", "Wheel")))
		.property("wheelRadius", &VehicleController::GetWheelRadius, &VehicleController::SetWheelRadius)
			(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::DRAG_FLOAT, 0.f, 10.f, "%.1f", "Wheel")))
		.property("wheelWidth", &VehicleController::GetWheelWidth, &VehicleController::SetWheelWidth)
			(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::DRAG_FLOAT, 0.f, 10.f, "%.1f", "Wheel")))
		.property("WheelOffset", &VehicleController::GetCenter, &VehicleController::SetCenter)
			(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::VEC3, 0.f, 10.f, "%.1f", "Wheel")))
		.property("Break Force", &VehicleController::GetMaxBreakTorque, &VehicleController::SetMaxBreakTorque)
			(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::DRAG_FLOAT, 0.f, 10000.f, "%.1f", "Wheel")))
		.property("Steering Angle", &VehicleController::GetMaxSteeringAngle, &VehicleController::SetMaxSteeringAngle)
			(rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::SLIDER_FLOAT, 0.1f, 90.f, "%.1f", "Wheel")));
}

void VehicleController::Create()
{
	if (!gameObject->physicalBody)
		gameObject->physicalBody = new Physicalbody(gameObject);

	// Create collision testers
	testers[0] = new JPH::VehicleCollisionTesterRay(static_cast<JPH::ObjectLayer>(Layers::MOVING));
	testers[1] = new JPH::VehicleCollisionTesterCastSphere(static_cast<JPH::ObjectLayer>(Layers::MOVING), 0.5f * wheelWidth);
	testers[2] = new JPH::VehicleCollisionTesterCastCylinder(static_cast<JPH::ObjectLayer>(Layers::MOVING));

	CreateBody();
	CreateCar();
}

void VehicleController::Destroy()
{
	DeleteCar();
	gameObject->physicalBody->RemoveBody(carBody);
	carBody = nullptr;
}

void VehicleController::SetParent(GameObject* go)
{
	IComponent::SetParent(go);
}

void VehicleController::OnDraw(Math::Vector3D camPos)
{
	Gizmos::color = { 0.f, 1.f, 0.f, 1.f };
	for (int i = 0; i < 4; ++i)
	{
		JPH::Mat44 joltMat = vehicleConstraint->GetWheelWorldTransform(i, JPH::Vec3::sAxisX(), JPH::Vec3::sAxisY());

		JPH::Vec3 joltPos = joltMat.GetTranslation();
		Math::Vector3D wheelWorldPos(joltPos.GetX(), joltPos.GetY(), joltPos.GetZ());

		JPH::Vec3 joltRight = joltMat.GetAxisX();
		JPH::Vec3 joltUp = joltMat.GetAxisY();
		JPH::Vec3 joltForward = joltMat.GetAxisZ();
		Math::Vector3D wheelRight(joltRight.GetX(), joltRight.GetY(), joltRight.GetZ());
		Math::Vector3D wheelUp(joltUp.GetX(), joltUp.GetY(), joltUp.GetZ());
		Math::Vector3D wheelForward(joltForward.GetX(), joltForward.GetY(), joltForward.GetZ());

		Gizmos::DrawWheel(wheelWorldPos, wheelRight, wheelUp, wheelForward, wheelRadius, wheelWidth);
	}
}



void VehicleController::SetDriverInput(const float inForward, const float inRight, const float inBrake, const float inHandBrake) const
{
	if (wheeledController)
		wheeledController->SetDriverInput(inForward, inRight, inBrake, inHandBrake);
}

float VehicleController::GetRPM() const
{
	return wheeledController->GetEngine().GetCurrentRPM();
}

float VehicleController::GetSpeed() const
{
	float velocityMS = carBody->GetBody()->GetLinearVelocity().Length();
	return velocityMS * 3.6f;
}

int VehicleController::GetCurrentGear()
{
	return wheeledController->GetTransmission().GetCurrentGear();
}

void VehicleController::SetCurrentGear(int currentGear)
{
	if (currentGear >= numGears || currentGear < -1)
		return;

	wheeledController->GetTransmission().Set(currentGear, 1.f);
	Debug::Log(ToString(currentGear));
}

void VehicleController::SetTransmissionMode(bool _automatic)
{
	wheeledController->GetTransmission().mMode = _automatic ? JPH::ETransmissionMode::Auto : JPH::ETransmissionMode::Manual;
	automatic = _automatic;
}



GameObject* VehicleController::GetCollidingObject() const
{
	return collidingObject;
}

void VehicleController::SetCollidingObject(GameObject* go)
{
	collidingObject = go;
	Rebuild();
}

void VehicleController::Rebuild()
{
	if (!gameObject || !gameObject->physicalBody)
		return;

	DeleteCar();
	if (carBody)
	{
		gameObject->physicalBody->RemoveBody(carBody);
		carBody = nullptr;
	}
	CreateBody();
	CreateCar();
}


void VehicleController::SetMass(const float _mass)
{
	if (_mass == 0)
		return;
	mass = _mass;
	Rebuild();
}

void VehicleController::SetMaxRollAngle(const float _rollAngle)
{
	maxRollAngle = JPH::DegreesToRadians(_rollAngle);
	DeleteCar();
	CreateCar();
}

void VehicleController::SetDriveType(int _driveType)
{
	if (_driveType == 0)
		driveType = FWD;
	else if (_driveType == 1)
		driveType = RWD;
	else if (_driveType == 2)
		driveType = AWD;
	DeleteCar();
	CreateCar();
}

void VehicleController::SetMaxSteeringAngle(const float _steerAngle)
{
	maxSteeringAngle = JPH::DegreesToRadians(_steerAngle);
	DeleteCar();
	CreateCar();
}

void VehicleController::SetFrontStiffness(const float stiffness)
{
	frontStiffness = stiffness;
	DeleteCar();
	CreateCar();
}

void VehicleController::SetBackStiffness(const float stiffness)
{
	backStiffness = stiffness;
	DeleteCar();
	CreateCar();
}

void VehicleController::SetFrequency(float _frequency)
{
	suspensionFrequency = _frequency;
	DeleteCar();
	CreateCar();
}

void VehicleController::SetDamping(float _damping)
{
	suspensionDamping = _damping;
	DeleteCar();
	CreateCar();
}

void VehicleController::SetMaxEngineToque(const float _maxEngineTorque)
{
	maxEngineTorque = _maxEngineTorque;
	DeleteCar();
	CreateCar();
}

void VehicleController::SetMinRPM(const float _minRPM)
{
	if (_minRPM < 0 || _minRPM >= maxRPM)
		return;

	minRPM = _minRPM;
	DeleteCar();
	CreateCar();
}

void VehicleController::SetMaxRPM(const float _maxRPM)
{
	if (_maxRPM < minRPM)
		return;

	maxRPM = _maxRPM;
	DeleteCar();
	CreateCar();
}

void VehicleController::SetAngularDamping(const float _angularDamping)
{
	angularDamping = _angularDamping;
	DeleteCar();
	CreateCar();
}

void VehicleController::SetInertiaMoment(const float _inertiaMoment)
{
	if (_inertiaMoment == 0)
		return;
	inertiaMoment = _inertiaMoment;
	DeleteCar();
	CreateCar();
}

void VehicleController::SetCenter(Math::Vector3D _center)
{
	this->center = _center;
	DeleteCar();
	CreateCar();
}

void VehicleController::SetWheelRadius(const float radius)
{
	if (radius <= 0)
		return;
	wheelRadius = radius;
	DeleteCar();
	CreateCar();
}

void VehicleController::SetWheelWidth(const float width)
{
	if (width <= 0)
		return;

	wheelWidth = width;
	DeleteCar();
	CreateCar();
}

void VehicleController::SetHalfVehicleLength(const float length)
{
	halfVehicleLength = length;
	DeleteCar();
	if (carBody)
	{
		gameObject->physicalBody->RemoveBody(carBody);
		carBody = nullptr;
	}
	CreateBody();
	CreateCar();
}

void VehicleController::SetHalfVehicleWidth(float width)
{
	halfVehicleWidth = width;
	DeleteCar();
	if (carBody)
	{
		gameObject->physicalBody->RemoveBody(carBody);
		carBody = nullptr;
	}
	CreateBody();
	CreateCar();
}

void VehicleController::SetMaxBreakTorque(float _maxBreakTorque)
{
	maxBrakeTorque = _maxBreakTorque;
	DeleteCar();
	CreateCar();
}



void VehicleController::CreateBody()
{
	// Create vehicle body
	Math::Vector3D vehiclePos = gameObject->transform.position;
	JPH::RVec3 position(vehiclePos.x, vehiclePos.y, vehiclePos.z);
	Math::Quaternion vehicleRot = gameObject->transform.rotation;
	JPH::Quat rotation(vehicleRot.x, vehicleRot.y, vehicleRot.z, vehicleRot.w);
	JPH::Vec3 offset(0, -0.5f, 0);
	JPH::ShapeRefC shape = EmptyCollider().GetShape();
	if (collidingObject != nullptr)
	{
		if (JPH::ShapeRefC convexShape = ConvexCollider::CreateShape(collidingObject))
			shape = convexShape;
	}

	JPH::Vec3 extent(halfVehicleWidth * 2, 1.f, halfVehicleLength * 2);

	JPH::RefConst car_shape = JPH::OffsetCenterOfMassShapeSettings(offset, shape).Create().Get();
	JPH::BodyCreationSettings car_body_settings = {
		car_shape,
		position,
		rotation,
		JPH::EMotionType::Dynamic,
		static_cast<JPH::ObjectLayer>(Layers::MOVING)
	};
	car_body_settings.mOverrideMassProperties = JPH::EOverrideMassProperties::MassAndInertiaProvided;
	car_body_settings.mMassPropertiesOverride.SetMassAndInertiaOfSolidBox(extent, mass);
	car_body_settings.mMassPropertiesOverride.mMass = mass;
	car_body_settings.mAllowSleeping = false;
	carBody = new Body(car_body_settings);
	gameObject->physicalBody->AddBody(carBody);
	gameObject->physicalBody->SetMotionType(MotionType::DYNAMIC);
}

void VehicleController::CreateCar()
{
	controller = new JPH::WheeledVehicleControllerSettings();

	//Vehicle part
	vehicle = new JPH::VehicleConstraintSettings();
	vehicle->mController = controller;
	vehicle->mMaxPitchRollAngle = maxRollAngle;
	CreateAntiRollbar();
	CreateWheels();

	//Controller part
	SetDifferential();
	SetEngine();
	SetTransmission();

	CreateVehicleConstraint();
}

void VehicleController::DeleteCar()
{
	if (vehicleConstraint)
	{
		Physics::GetInstance().GetPhysicsSystem()->RemoveStepListener(vehicleConstraint);
		Physics::GetInstance().GetPhysicsSystem()->RemoveConstraint(vehicleConstraint);
	}

	vehicleConstraint = nullptr;
	vehicle = nullptr;
	controller = nullptr;
	wheeledController = nullptr;
}

void VehicleController::CreateWheels()
{
	float xOffset = center.x;
	float yOffset = center.y;
	float zOffset = center.z;

	JPH::Array<JPH::Ref<JPH::WheelSettings>> wheels(4);
	JPH::Vec3 normalX(1, 1, 1);
	JPH::Vec3 flipX(-1, 1, 1);

	// Wheels, left front
	JPH::Vec3 pos = JPH::Vec3(xOffset + halfVehicleWidth, yOffset, zOffset + halfVehicleLength);
	wheels[0] = CreateFrontWheel(pos, normalX);

	// Right front
	pos = JPH::Vec3(xOffset - halfVehicleWidth, yOffset, zOffset + halfVehicleLength);
	wheels[1] = CreateFrontWheel(pos, flipX);

	// Left rear
	pos = JPH::Vec3(xOffset + halfVehicleWidth, yOffset, zOffset - halfVehicleLength);
	wheels[2] = CreateRearWheel(pos, normalX);

	// Right rear
	pos = JPH::Vec3(xOffset - halfVehicleWidth, yOffset, zOffset - halfVehicleLength);
	wheels[3] = CreateRearWheel(pos, flipX);

	vehicle->mWheels = wheels;
	for (JPH::WheelSettings* w : vehicle->mWheels)
	{
		w->mRadius = wheelRadius;
		w->mWidth = wheelWidth;
	}
}

JPH::WheelSettingsWV* VehicleController::CreateFrontWheel(JPH::Vec3 position, JPH::Vec3 flip_x)
{
	// Suspension direction
	JPH::Vec3 front_suspension_dir = JPH::Vec3(JPH::Tan(frontSuspensionSidewaysAngle), -1, JPH::Tan(frontSuspensionForwardAngle)).Normalized();
	JPH::Vec3 front_steering_axis = JPH::Vec3(-JPH::Tan(frontKingPinAngle), 1, -JPH::Tan(frontCasterAngle)).Normalized();
	JPH::Vec3 front_wheel_up = JPH::Vec3(JPH::Sin(frontCamber), JPH::Cos(frontCamber), 0);
	JPH::Vec3 front_wheel_forward = JPH::Vec3(-JPH::Sin(frontToe), 0, JPH::Cos(frontToe));

	JPH::WheelSettingsWV* w = new JPH::WheelSettingsWV();
	w->mPosition = position;
	w->mSuspensionDirection = flip_x * front_suspension_dir;
	w->mSteeringAxis = flip_x * front_steering_axis;
	w->mWheelUp = flip_x * front_wheel_up;
	w->mWheelForward = flip_x * front_wheel_forward;
	w->mSuspensionMinLength = suspensionMinLength;
	w->mSuspensionMaxLength = suspensionMaxLength;
	w->mSuspensionSpring.mFrequency = suspensionFrequency;
	w->mSuspensionSpring.mDamping = suspensionDamping;
	w->mMaxSteerAngle = maxSteeringAngle;
	w->mMaxHandBrakeTorque = 0.0f; // Front wheel doesn't have hand brake

	w->mLateralFriction.Clear();
	w->mLateralFriction.Reserve(4);
	w->mLateralFriction.AddPoint(0.0f, 0.0f);
	w->mLateralFriction.AddPoint(3.0f, 1.5f);
	w->mLateralFriction.AddPoint(8.0f, 1.2f);
	w->mLateralFriction.AddPoint(20.0f, 1.f);

	w->mLongitudinalFriction.Clear();
	w->mLongitudinalFriction.Reserve(3);
	w->mLongitudinalFriction.AddPoint(0.0f, 0.0f);
	w->mLongitudinalFriction.AddPoint(0.1f, 2.5f);
	w->mLongitudinalFriction.AddPoint(0.5f, 2.0f);

	return w;
}

JPH::WheelSettingsWV* VehicleController::CreateRearWheel(JPH::Vec3 position, JPH::Vec3 flip_x)
{
	// Suspension direction
	JPH::Vec3 rear_suspension_dir = JPH::Vec3(JPH::Tan(rearSuspensionSidewaysAngle), -1, JPH::Tan(rearSuspensionForwardAngle)).Normalized();
	JPH::Vec3 rear_steering_axis = JPH::Vec3(-JPH::Tan(rearKingPinAngle), 1, -JPH::Tan(rearCasterAngle)).Normalized();
	JPH::Vec3 rear_wheel_up = JPH::Vec3(JPH::Sin(rearCamber), JPH::Cos(rearCamber), 0);
	JPH::Vec3 rear_wheel_forward = JPH::Vec3(-JPH::Sin(rearToe), 0, JPH::Cos(rearToe));

	JPH::WheelSettingsWV* w = new JPH::WheelSettingsWV;
	w->mPosition = position;
	w->mSuspensionDirection = flip_x * rear_suspension_dir;
	w->mSteeringAxis = flip_x * rear_steering_axis;
	w->mWheelUp = flip_x * rear_wheel_up;
	w->mWheelForward = flip_x * rear_wheel_forward;
	w->mSuspensionMinLength = suspensionMinLength;
	w->mSuspensionMaxLength = suspensionMaxLength;
	w->mSuspensionSpring.mFrequency = suspensionFrequency;
	w->mSuspensionSpring.mDamping = suspensionDamping;
	w->mMaxSteerAngle = 0.0f;
	w->mMaxBrakeTorque = maxBrakeTorque; // Freinage ultra puissant

	w->mLateralFriction.Clear();
	w->mLateralFriction.Reserve(4);
	w->mLateralFriction.AddPoint(0.0f, 0.0f);
	w->mLateralFriction.AddPoint(3.0f, 1.6f);
	w->mLateralFriction.AddPoint(8.0f, 1.3f);
	w->mLateralFriction.AddPoint(20.0f, 1.1f);

	w->mLongitudinalFriction.Clear();
	w->mLongitudinalFriction.Reserve(3);
	w->mLongitudinalFriction.Clear();
	w->mLongitudinalFriction.Reserve(3);
	w->mLongitudinalFriction.AddPoint(0.0f, 0.0f);
	w->mLongitudinalFriction.AddPoint(0.1f, 2.5f);
	w->mLongitudinalFriction.AddPoint(0.5f, 2.0f);

	return w;
}

void VehicleController::CreateAntiRollbar() const
{
	vehicle->mAntiRollBars.resize(2);
	vehicle->mAntiRollBars[0].mLeftWheel = 0;
	vehicle->mAntiRollBars[0].mRightWheel = 1;
	vehicle->mAntiRollBars[1].mLeftWheel = 2;
	vehicle->mAntiRollBars[1].mRightWheel = 3;

	vehicle->mAntiRollBars[0].mStiffness = frontStiffness;
	vehicle->mAntiRollBars[1].mStiffness = backStiffness;
}

void VehicleController::SetDifferential() const
{
	if (driveType == AWD)
	{
		controller->mDifferentials.resize(2);
		controller->mDifferentials[0].mLeftWheel = 0;
		controller->mDifferentials[0].mRightWheel = 1;
		controller->mDifferentials[1].mLeftWheel = 2;
		controller->mDifferentials[1].mRightWheel = 3;

		controller->mDifferentials[0].mEngineTorqueRatio = 0.5f;
		controller->mDifferentials[1].mEngineTorqueRatio = 0.5f;
	}
	else if (driveType == FWD)
	{
		controller->mDifferentials.resize(1);
		controller->mDifferentials[0].mLeftWheel = 0;
		controller->mDifferentials[0].mRightWheel = 1;
		controller->mDifferentials[0].mEngineTorqueRatio = 1.f;
	}
	else if (driveType == RWD)
	{
		controller->mDifferentials.resize(1);
		controller->mDifferentials[0].mLeftWheel = 2;
		controller->mDifferentials[0].mRightWheel = 3;
		controller->mDifferentials[0].mEngineTorqueRatio = 1.f;
	}
}

void VehicleController::SetEngine() const
{
	controller->mEngine.mMaxTorque = maxEngineTorque;
	controller->mEngine.mMinRPM = minRPM;
	controller->mEngine.mMaxRPM = maxRPM;
	controller->mEngine.mInertia = inertiaMoment;
	controller->mEngine.mAngularDamping = angularDamping;

	controller->mEngine.mNormalizedTorque.Clear();
	controller->mEngine.mNormalizedTorque.AddPoint(0.0f, 0.4f); // 40% de puissance au ralenti
	controller->mEngine.mNormalizedTorque.AddPoint(0.5f, 1.0f); // 100% à mi-régime
	controller->mEngine.mNormalizedTorque.AddPoint(1.0f, 0.7f); // Baisse de puissance au rupteur
}

void VehicleController::SetTransmission() const
{
	if (automatic)
		controller->mTransmission.mMode = JPH::ETransmissionMode::Auto;
	else
		controller->mTransmission.mMode = JPH::ETransmissionMode::Manual;
	controller->mTransmission.mSwitchLatency = 0.2f;
	controller->mTransmission.mGearRatios = {  2.60f, 2.10f, 1.70f, 1.35f, 1.15f, 0.95f, 0.80f };
	controller->mTransmission.mReverseGearRatios = { -3.0f };
}

void VehicleController::CreateVehicleConstraint()
{
	if (vehicleConstraint != nullptr)
	{
		Physics::GetInstance().GetPhysicsSystem()->RemoveStepListener(vehicleConstraint);
		Physics::GetInstance().GetPhysicsSystem()->RemoveConstraint(vehicleConstraint);
	}

	// Create vehicle constraint
	vehicleConstraint = new JPH::VehicleConstraint(*carBody->GetBody(), *vehicle);
	vehicleConstraint->SetVehicleCollisionTester(testers[0]);

	wheeledController = static_cast<JPH::WheeledVehicleController*>(vehicleConstraint->GetController());

	Physics::GetInstance().GetPhysicsSystem()->AddConstraint(vehicleConstraint);
	Physics::GetInstance().GetPhysicsSystem()->AddStepListener(vehicleConstraint);
}
