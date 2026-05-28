#include "MainMenu.h"
#include "components/button.h"
#include "engine/engine.h"

void MainMenu::Start()
{
    if (camera1) cameras.push_back(camera1->GetComponent<CameraMainMenu>());
    if (camera2) cameras.push_back(camera2->GetComponent<CameraMainMenu>());
    if (camera3) cameras.push_back(camera3->GetComponent<CameraMainMenu>());
    if (ChooseCarObject) chooseCarScript = ChooseCarObject->GetComponent<ChooseCar>();
    // Set up button callbacks
    if (playButton)
        playButton->GetComponent<Button>()->SetOnClick([this] { SelectCar(); });

    if (quitButton)
        quitButton->GetComponent<Button>()->SetOnClick([] { Engine::RequestStopGame(); });

    Restart();
}

void MainMenu::Update(float deltaTime)
{
    if (chooseCar)
        return;
    if (cameras[actualCameraIndex])
    {
        cameras[actualCameraIndex]->UpdateCamera(deltaTime);
        if (cameras[actualCameraIndex]->Reset())
        {
            actualCameraIndex = (actualCameraIndex + 1) % cameras.size();
            currentScene->SetGameCam(cameras[actualCameraIndex]->gameObject);
        }
    }
}

void MainMenu::Restart()
{
    chooseCar = false;
    actualCameraIndex = 0;
    currentScene = SceneManager::GetInstance().GetCurrentScene();
    if (currentScene)
        currentScene->SetGameCam(cameras[actualCameraIndex]->gameObject);
    for (CameraMainMenu* cam : cameras)
    {
        if (cam)
            cam->ResetAll();
    }
}

void MainMenu::SelectCar()
{
    if(!chooseCarScript)
        return;
    
    chooseCarScript->Init();
    chooseCar = true;
    // Move play button out of view
    playButton->transform.position = Vector3D(0.f, 1000.f, 0.f);
    quitButton->transform.position = Vector3D(0.f, 1000.f, 0.f);
}
