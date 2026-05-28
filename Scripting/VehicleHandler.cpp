#include "VehicleHandler.h"
#include "components/vehicle_controller.h"
#include "physics/physics.h"
#include "window/input_manager.h"

void VehicleHandler::Update(float deltaTime)
{
	if (!gameManager->gameLaunched)
		return;

	HandleVehicleInputs();
	UpdateVehicleSound();
	UpdateVehicleProperties();
}

void VehicleHandler::End()
{
	if (soundEmitter)
		soundEmitter->ClearSounds();

	for (Sound* sound : RPM)
		delete sound;
	RPM.clear();
	soundEmitter = nullptr;
}

void VehicleHandler::Init(GameObject* _manager)
{
	controller = GetComponent<VehicleController>();
	rpmText = GetComponent<Text>();
	playerHandler = GetComponent<PlayerHandler>();
	InitSounds();
	if (_manager)
		gameManager = _manager->GetComponent<GameManager>();
}

void VehicleHandler::HandleVehicleInputs()
{
	if (!controller)
	{
		Debug::LogWarning("VehicleHandler: No VehicleController component found on " + gameObject->GetName());
		return;
	}

	float speed = controller->GetSpeed();
	float steerFactor = std::clamp(1.0f - (speed / 200.0f), 0.2f, 1.0f);

	float forward = 0.0f;
	float right = 0.0f;
	float brake = 0.0f;
	float handBrake = 0.0f;

	if (InputManager::GetKeyDown(KEY_UP_ARROW))
		forward = 1.0f;
	if (InputManager::GetKeyDown(KEY_DOWN_ARROW))
	{
		brake = 1.0f;
		playerHandler->Brake(true);
	}
	else
		playerHandler->Brake(false);
	if (InputManager::GetKeyDown(KEY_RIGHT_ARROW))
		right = 1.0f * steerFactor;
	if (InputManager::GetKeyDown(KEY_LEFT_ARROW))
		right = -1.0f * steerFactor;

	controller->SetDriverInput(forward, right, brake, handBrake);

	//Handle Gear Changement
	if (InputManager::GetKeyClicked(KEY_M))
		controller->SetTransmissionMode(true);
	if (InputManager::GetKeyClicked(KEY_P))
		controller->SetTransmissionMode(false);

	if (!controller->IsAutomatic())
	{
		int currentGear = controller->GetCurrentGear();
		if (InputManager::GetKeyClicked(KEY_LEFT_SHIFT))
		{
			currentGear += 1;
			controller->SetCurrentGear(currentGear);
		}
		if (InputManager::GetKeyClicked(KEY_LEFT_CONTROL))
		{
			currentGear -= 1;
			controller->SetCurrentGear(currentGear);
		}
	}
}

void VehicleHandler::UpdateVehicleProperties() const
{
	float speed = controller->GetSpeed();
	float downforceCoeff = 2.0f;

	Vector3D force = - gameObject->transform.worldRotation.Up() * downforceCoeff * speed;
	gameObject->physicalBody->AddForce(force);
}

void VehicleHandler::UpdateVehicleSound()
{
	if (!controller || RPM.empty())
		return;

	float rpm = controller->GetRPM();
	if (rpmText)
		rpmText->SetText("RPM: " + ToString(rpm));

	float baseVolume = 1.0f; 
	if (soundEmitter)
		baseVolume = soundEmitter->GetVolume();

	if (rpm < lastRPM) {
		baseVolume *= 0.8f;
	}

	UpdateEngineWork(rpm, baseVolume);

	lastRPM = rpm;
}

void VehicleHandler::UpdateEngineWork(float rpm, float masterVolume)
{
	float floatIndex = rpm / rpmGap;
	int indexA = static_cast<int>(std::floor(floatIndex));
	int indexB = indexA + 1;

	float blendFactor = floatIndex - static_cast<float>(indexA);

	for (int i = 0; i < (int)RPM.size(); ++i)
	{
		if (i == indexA)
		{
			float targetPitch = rpm / (static_cast<float>(i) * rpmGap + 0.001f);
			if (i == 0) targetPitch = rpm / (rpmGap * 0.5f); // Eviter division par zéro

			RPM[i]->SetVolume((1.0f - blendFactor) * masterVolume);
			RPM[i]->SetPitch(targetPitch);
		}
		else if (i == indexB)
		{
			float targetPitch = rpm / (static_cast<float>(i) * rpmGap);
            
			RPM[i]->SetVolume(blendFactor * masterVolume);
			RPM[i]->SetPitch(targetPitch);
		}
		else
		{
			RPM[i]->SetVolume(0.0f);
		}
	}
}

void VehicleHandler::InitSounds()
{
	RPM.push_back(new Sound("assets/sounds/mazda_mx5/0K_RPM_loop.mp3"));
	RPM.push_back(new Sound("assets/sounds/mazda_mx5/1K_RPM_loop.mp3"));
	RPM.push_back(new Sound("assets/sounds/mazda_mx5/2K_RPM_loop.mp3"));
	RPM.push_back(new Sound("assets/sounds/mazda_mx5/3K_RPM_loop.mp3"));
	RPM.push_back(new Sound("assets/sounds/mazda_mx5/4K_RPM_loop.mp3"));
	RPM.push_back(new Sound("assets/sounds/mazda_mx5/5K_RPM_loop.mp3"));
	RPM.push_back(new Sound("assets/sounds/mazda_mx5/6K_RPM_loop.mp3"));
	RPM.push_back(new Sound("assets/sounds/mazda_mx5/7K_RPM_loop.mp3"));

	soundEmitter = GetComponent<SoundEmitter>();
	if (!soundEmitter)
	{
		Debug::Log("No Emitter");
		return;
	}

	for (Sound* sound : RPM)
	{
		soundEmitter->AddSound(sound);
		sound->Spatialize(true);
		sound->Play(true);
		sound->Loop(true);
		sound->SetVolume(0.f);
	}

	actualVolume = 10.f;
}
