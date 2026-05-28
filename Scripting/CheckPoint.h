#pragma once
#include "scripting/script_base.h"

class CheckPoint : public Script<CheckPoint>
{
    ENIGMA_CLASS(CheckPoint)
public:
    void OnCollisionEnter(Physicalbody& body1, Physicalbody& body2) override;
    
    ENIGMA_PROPERTY()
    GameObject* manager;
    ENIGMA_PROPERTY()
    bool end = false;
};