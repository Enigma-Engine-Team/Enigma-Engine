#include "CheckPoint.h"
#include "GameManager.h"
#include "PlayerHandler.h"

void CheckPoint::OnCollisionEnter(Physicalbody& body1, Physicalbody& body2)
{
    if (PlayerHandler* playerHandler = body2.gameObject->GetComponent<PlayerHandler>())
    {
        if (end && playerHandler->IsFinished())
        {
            std::string name = playerHandler->gameObject->GetName();
            manager->GetComponent<GameManager>()->EndGame(name);
        }
        if (!end)
            playerHandler->CollideCheckPoint();
    }
}