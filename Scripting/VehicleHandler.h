#pragma once
#include "GameManager.h"
#include "PlayerHandler.h"
#include "components/text.h"
#include "components/vehicle_controller.h"
#include "scripting/script_base.h"
#include "sound/sound.h"
#include "sound/sound_emitter.h"

class VehicleHandler : public Script<VehicleHandler>
{
    ENIGMA_CLASS(VehicleHandler)
public:
    void Update(float deltaTime) override;
    void End() override;
    void Init(GameObject* _manager);

private:
    void HandleVehicleInputs();
    void UpdateVehicleProperties() const;
    void UpdateVehicleSound();
    void UpdateEngineWork(float rpm, float masterVolume);
    void InitSounds();

    VehicleController* controller = nullptr;
    SoundEmitter* soundEmitter = nullptr;
    Text* rpmText = nullptr;
    GameManager* gameManager = nullptr;
    PlayerHandler* playerHandler = nullptr;

    std::vector<Sound*> RPM;

    float lastRPM = 0.0f;
    float maxRPM = 7000.f;
    float rpmGap = 1000.f;
    float actualVolume = 1.f;
};
