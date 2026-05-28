#pragma once
#include "scripting/script_base.h"

class CameraMainMenu : public Script<CameraMainMenu>
{
    ENIGMA_CLASS(CameraMainMenu)
public:
    void UpdateCamera(float deltaTime);
    bool Reset();
    void ResetAll();
    ENIGMA_PROPERTY()
    Vector3D firstPosition;
    ENIGMA_PROPERTY()
    Vector3D firstRotation;
    ENIGMA_PROPERTY()
    Vector3D lastPosition;
    ENIGMA_PROPERTY()
    Vector3D lastRotation;
    ENIGMA_PROPERTY()
    float lerpTime;
    
private:
    float actualLerpTime = 0.f;
    float t = 0.f;
};
