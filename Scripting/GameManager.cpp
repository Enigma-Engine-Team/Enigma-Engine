#include "GameManager.h"
#include <scenes/scene_manager.h>
#include "MainMenu.h"
#include "PlayerHandler.h"
#include "Scoreboard.h"
#include "VehicleHandler.h"
#include "physics/physics.h"

void GameManager::Awake()
{
    Physics::GetInstance().SetActive(false);
    gameTimer = countdownNum->GetComponent<Text>();
}

void GameManager::Update(float deltaTime)
{
    if (startCountDown)
    {
        CountDown(deltaTime);
    }

    Frame frame = Network::GetInstance().GetFrame();
    if (!frame.IsEmpty())
    {
        HandlePacket(frame);
    }
}

void GameManager::PlaceVehicle(GameObject* go)
{
    vehicles.push_back(go);
    VehicleHandler* vHandler = go->AddComponent<VehicleHandler>();
    vHandler->Init(this->gameObject);
    
    go->transform.SetRotation(Vector3D(180.f, 90.f, 180.f));
    if (Network::GetInstance().GetClient()->GetID() == 0)
        go->transform.SetPosition(Vector3D(100.62f, 6.83f, -183.75f));
    else
        go->transform.SetPosition(Vector3D(100.62f, 6.83f, -177.75f));
    
    if (GameCamera* gameCam = go->GetComponentInChildren<GameCamera>())
        SceneManager::GetInstance().GetCurrentScene()->SetGameCam(gameCam);
}

void GameManager::EndGame(const std::string& winnerName)
{
    for (GameObject* vehicle : vehicles)
    {
        vehicle->GetComponent<PlayerHandler>()->Restart();
        vehicle->GetComponent<VehicleHandler>()->End();
        vehicle->DeleteScript("VehicleHandler");
    }

    Physics::GetInstance().SetActive(false);
    GetComponent<MainMenu>()->Restart();
    scoreboard->GetComponent<Scoreboard>()->ShowScoreBoard(winnerName);
    Reset();
}

void GameManager::StartCountDown()
{
    startCountDown = true;
    gameTimer->gameObject->transform.position = Vector3D(0.f, 0.f, 0.f);
}

void GameManager::CountDown(float deltaTime)
{
    countDown += deltaTime;
    int intCountDown = static_cast<int>(countDown);
    if (intCountDown >= 4)
    {
        countDown = 0.f;
        startCountDown = false;
        gameTimer->gameObject->transform.position = Vector3D(0.f, -2000.f, 0.f);
        StartGame();
    }
    else
    {
        gameTimer->SetText(ToString(3 - intCountDown));
        if (intCountDown == 3)
            gameTimer->SetText("GO");
    }
}

void GameManager::StartGame()
{
    Physics::GetInstance().SetActive(true);
    gameLaunched = true;
}

void GameManager::Reset()
{
    countDown = 0.f;
    startCountDown = false;
    vehicles.clear();
}
void GameManager::HandlePacket(Frame& frame)
{
    int id = frame.ReadUInt8_t();

    while (!frame.IsTraveled())
    {
        switch (static_cast<EPacket>(frame.ReadUInt8_t()))
        {
        case EPacket::PLAYER_CONNECTION:
            HandleNewPlayer(id, frame);
            break;
        case EPacket::PLAYER_DISCONNECTION:
            HandleRemovePlayer(id, frame);
            break;
        case EPacket::PLAYER_POSITION:
            HandlePlayerPosition(id, frame);
            break;
        case EPacket::PLAYER_ROTATION:
            HandlePlayerRotation(id, frame);
            break;
        default:
            break;
        }
    }
}

void GameManager::HandleNewPlayer(int id, Frame& frame)
{
    if (otherPlayers.contains(id))
    {
        return;
    }
    GameObject* go = id == 0 ? car2 : car1;
    otherPlayers.insert({ id, go });
    Network::GetInstance().UpdateStatus(Client::PRESENT);
}

void GameManager::HandleRemovePlayer(int id, Frame& frame)
{
    if (!otherPlayers.contains(id))
    {
        return;
    }
    otherPlayers.at(id)->transform.scale = Vector3D::Zero;
    otherPlayers.erase(id);

}

void GameManager::HandlePlayerPosition(int id, Frame& frame)
{
    Math::Vector3D vec3 = frame.ReadVector3();
    if (otherPlayers.contains(id))
    {
        otherPlayers.at(id)->transform.position = vec3;
    }
}

void GameManager::HandlePlayerRotation(int id, Frame& frame)
{
    Math::Quaternion quat = frame.ReadQuaternion();
    if (otherPlayers.contains(id))
    {
        otherPlayers.at(id)->transform.rotation = quat;
    }
}