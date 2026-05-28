#pragma once
#include "scripting/iscript.h"

struct CollisionManager
{
    std::vector<Scripting::IScript*> collisionEnter;
    std::vector<Scripting::IScript*> collisionStay;
    std::vector<Scripting::IScript*> collisionExit;
};
