#include "ChooseCar.h"

#include <components/button.h>
#include <network/network.h>
#include "GameManager.h"
#include "PlayerHandler.h"
#include "scenes/scene_manager.h"

void ChooseCar::Awake()
{
    if (chooseCarCamera)
        cameraScript = chooseCarCamera->GetComponent<GameCamera>();
    
    if (switchCarButton)
        switchCarButton->GetComponent<Button>()->SetOnClick([this] { SwitchCar(); });
    if (chooseCarButton)
    {
        chooseCarButton->GetComponent<Button>()->SetOnClick([this]
        {
            GameObject* carSelected = cars[carIndex];
            GetComponent<GameManager>()->PlaceVehicle(carSelected);
            GetComponent<GameManager>()->player = carSelected;
            GetComponent<GameManager>()->StartCountDown();
            switchCarButton->transform.position = Vector3D(0.f, -2000.f, 0.f);
            chooseCarButton->transform.position = Vector3D(800.f, -2000.f, 0.f);
            Client::Message messagePos = {
                	.type = EPacket::PLAYER_POSITION,
                	.data = &carSelected->transform.position,
                	.reliability = Client::UNRELIABLE
                };
            Client::Message messageRot = {
                    .type = EPacket::PLAYER_ROTATION,
                    .data = &carSelected->transform.rotation,
                    .reliability = Client::UNRELIABLE
            };
            Network::GetInstance().GetClient()->AddMessage(messagePos);
            Network::GetInstance().GetClient()->AddMessage(messageRot);
        });
    }

    chosenCar = car1; // Default car
    positions.push_back(firstPosition);
    positions.push_back(secondPosition);
    positions.push_back(thirdPosition);
    rotations.push_back(firstRotation);
    rotations.push_back(secondRotation);
    rotations.push_back(thirdRotation);
    cars.push_back(car1);
    cars.push_back(car2);
    cars.push_back(car3);
    
    lerpPos = gameObject->transform.position;
    lerpRot = gameObject->transform.rotation;
}

void ChooseCar::Update(float deltaTime)
{
    t += deltaTime;
    t = (((t) < (lerpTime)) ? (t) : (lerpTime));
    cameraScript->gameObject->transform.position = Vector3D::Lerp(lerpPos, positions[carIndex], t);
    cameraScript->gameObject->transform.rotation = Quaternion::Slerp(lerpRot, Quaternion::FromEuler(rotations[carIndex]), t);
}

void ChooseCar::Init() const
{
    SceneManager::GetInstance().GetCurrentScene()->SetGameCam(cameraScript->gameObject);
    switchCarButton->transform.position = Vector3D(0.f, -450.f, 0.f);
    chooseCarButton->transform.position = Vector3D(800.f, -450.f, 0.f);

    for (GameObject* car : cars)
        car->GetComponent<PlayerHandler>()->Restart();
}

void ChooseCar::SwitchCar()
{
    carIndex++;
    if (carIndex >= static_cast<int>(cars.size()))
        carIndex = 0;
    
    chosenCar = cars[carIndex];
    lerpPos = cameraScript->gameObject->transform.position;
    lerpRot = cameraScript->gameObject->transform.rotation;
    t = 0.f;
}
