#pragma once
#include "scripting/script_base.h"

class Test : public Script<Test>
{
    ENIGMA_CLASS(Test)
public:
    ENIGMA_PROPERTY()
    float collisionPower = 5.0f;
    
    void OnCollisionEnter(Physicalbody& body1, Physicalbody& body2) override;
};
