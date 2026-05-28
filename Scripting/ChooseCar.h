#pragma once
#include "scripting/script_base.h"
#include <components/game_camera.h>

class ChooseCar : public Script<ChooseCar>
{
    ENIGMA_CLASS(ChooseCar)
public:
    enum EPacket : uint8_t
    {
        PLAYER_CONNECTION,
        PLAYER_DISCONNECTION,
        PLAYER_POSITION,
        PLAYER_ROTATION
    };

    void Awake() override;
    void Update(float deltaTime) override;
    void Init() const;
    GameObject* GetChosenCar() const { return chosenCar; }
    ENIGMA_PROPERTY()
    GameObject* switchCarButton;    
    ENIGMA_PROPERTY()
    GameObject* chooseCarButton;
    ENIGMA_PROPERTY()
    GameObject* chooseCarCamera;
    ENIGMA_PROPERTY()
    GameObject* car1;
    ENIGMA_PROPERTY()
    Vector3D firstPosition;
    ENIGMA_PROPERTY()
    Vector3D firstRotation;
    ENIGMA_PROPERTY()
    GameObject* car2;
    ENIGMA_PROPERTY()
    Vector3D secondPosition;
    ENIGMA_PROPERTY()
    Vector3D secondRotation;
    ENIGMA_PROPERTY()
    GameObject* car3;
    ENIGMA_PROPERTY()
    Vector3D thirdPosition;
    ENIGMA_PROPERTY()
    Vector3D thirdRotation;
private:
    void SwitchCar();
    GameObject* chosenCar = nullptr;
    GameCamera* cameraScript = nullptr;
    
    std::vector<GameObject*> cars;
    std::vector<Vector3D> positions;
    std::vector<Vector3D> rotations;
    
    Vector3D lerpPos;
    Quaternion lerpRot;
    
    float t = 0.f;
    float lerpTime = 1.f;
    int carIndex = 0;
};
