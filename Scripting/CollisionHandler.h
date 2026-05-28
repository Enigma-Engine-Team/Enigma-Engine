#pragma once
#include "scripting/script_base.h"

class CollisionHandler : public Script<CollisionHandler>
{
    ENIGMA_CLASS(CollisionHandler)
public:
    void OnCollisionEnter(Physicalbody& body1, Physicalbody& body2) override;
    
    ENIGMA_PROPERTY()
    float impulseForce = 10.f;
};