#pragma once
#include "scripting/iscript.h"
#include "../utilities/macro.h"
#include "time.h"
#include "scripting/scripting.h"

using namespace Math;

template <typename T>
class Script : public Scripting::IScript
{
public:
    void Create() final;
protected:
    Script() = default;
};

#include "script_base.inl"
