#pragma once
#include "components/text.h"
#include <unordered_map>
#include "network/network.h"
#include "scripting/script_base.h"

class GameManager : public Script<GameManager>
{
    ENIGMA_CLASS(GameManager)
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
    void PlaceVehicle(GameObject* go);
    void EndGame(const std::string& winnerName);
    void StartCountDown();

    ENIGMA_PROPERTY()
    GameObject* scoreboard;
    ENIGMA_PROPERTY()
    GameObject* countdownNum;
    ENIGMA_PROPERTY()
    GameObject* car1;
    ENIGMA_PROPERTY()
    GameObject* car2;
    ENIGMA_PROPERTY()
    GameObject* car3;
    
    bool gameLaunched = false;
    GameObject* player = nullptr;

private:

    void CountDown(float deltaTime);
    void StartGame();
    void Reset();
    void HandlePacket(Frame& frame);
    void HandleNewPlayer(int id, Frame& frame);
    void HandleRemovePlayer(int id, Frame& frame);
    void HandlePlayerPosition(int id, Frame& frame);
    void HandlePlayerRotation(int id, Frame& frame);
    std::vector<GameObject*> vehicles;
    Text* gameTimer = nullptr;
    float countDown = 0.f;
    bool startCountDown = false;
    std::unordered_map<int, GameObject*> otherPlayers;
};