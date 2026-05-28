#include "scripting/iscript.h"

#include "scripting/script_manager.h"

void Scripting::IScript::Destroy()
{
    ScriptManager::GetInstance().UnregisterScript(this);
}
