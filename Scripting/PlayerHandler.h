#pragma once
#include "scripting/script_base.h"

class PlayerHandler : public Script<PlayerHandler>
{
    ENIGMA_CLASS(PlayerHandler)
public:
    void CollideCheckPoint();
    void Brake(bool brake);
    bool IsFinished() const { return finished; }
    void Restart();
    ENIGMA_PROPERTY()
    GameObject* brakeLight;
    ENIGMA_PROPERTY()
    Vector3D idlePos;
    ENIGMA_PROPERTY()
    Vector3D idleRot;
    ENIGMA_PROPERTY()
    int numCheckPoint = 2;
private:
    bool finished = false;
    int actualCheckPoint = 0;
};