#pragma once 
#include "scripting/script_base.h"
#include "scenes/scene_manager.h"
#include "CameraMainMenu.h"
#include "ChooseCar.h"

class MainMenu : public Script<MainMenu>
{
    ENIGMA_CLASS(MainMenu)
public:
    void Start() override;
    void Update(float deltaTime) override;
    void Restart();
    
    ENIGMA_PROPERTY()
    GameObject* playButton;
    ENIGMA_PROPERTY()
    GameObject* quitButton;
    ENIGMA_PROPERTY()
    GameObject* camera1;
    ENIGMA_PROPERTY()
    GameObject* camera2;
    ENIGMA_PROPERTY()
    GameObject* camera3;
    ENIGMA_PROPERTY()
    GameObject* ChooseCarObject;
    
private:
    void SelectCar();
    std::vector<CameraMainMenu*> cameras;
    Scene* currentScene = nullptr;
    ChooseCar* chooseCarScript = nullptr;
    int actualCameraIndex = 0;
    bool chooseCar = false;
};