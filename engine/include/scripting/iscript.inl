#pragma once

template<typename T>
requires std::derived_from<T, IComponent>
T* Scripting::IScript::GetComponent() const
{
	return gameObject->GetComponent<T>();
}

template<typename T>
requires std::derived_from<T, IComponent>
T* Scripting::IScript::AddComponent() const
{
	return gameObject->AddComponent<T>();
}