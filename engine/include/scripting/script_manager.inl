#pragma once
template <typename T>
requires std::derived_from<T, Scripting::IScript>
void ScriptManager::RegisterScript(T* instance)
{
    scripts.push_back(instance);
    awake.push_back(instance);
    start.push_back(instance);
    update.push_back(instance);
    fixedUpdate.push_back(instance);
    lateUpdate.push_back(instance);
    end.push_back(instance);
    collisionEnter.push_back(instance);
    collisionStay.push_back(instance);
    collisionExit.push_back(instance);
}