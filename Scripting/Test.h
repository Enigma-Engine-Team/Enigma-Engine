#pragma once
#include "scripting/script_base.h"


class Test : public Script<Test>
{
    ENIGMA_CLASS(Test)
public:
    ENIGMA_PROPERTY()
    float speed = 5.0f;
};
