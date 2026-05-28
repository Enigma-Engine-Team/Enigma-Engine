#pragma once
#include "script_manager.h"

template<typename T>
void Script<T>::Create()
{
    ScriptManager::GetInstance().RegisterScript<T>(static_cast<T*>(this));
}