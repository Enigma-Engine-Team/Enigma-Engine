#include "debug/log.h"

template<typename T>
requires std::derived_from<T, IComponent>
T* GameObject::AddComponent()
{
	if (IComponent* component = new T())
	{
		component->SetParent(this);
		component->Create();
		components.push_back(component);

		return dynamic_cast<T*>(component);
	}
	Debug::LogError("Add component failed");
	return nullptr;
}

template<typename T>
requires std::derived_from<T, IComponent>
T* GameObject::GetComponent()
{
	for (IComponent* component : components)
	{
		if (typeid(*component) == typeid(T))
		{
			return dynamic_cast<T*>(component);
		}
	}
	return nullptr;
}

template<typename T> requires std::derived_from<T, IComponent>
T* GameObject::GetComponentInChildren()
{
	for (auto go : children)
	{
		for (IComponent* component : go->GetComponents())
		{
			if (typeid(*component) == typeid(T))
				return dynamic_cast<T*>(component);
		}
	}
	return nullptr;
}

inline std::vector<IComponent*> GameObject::GetComponents()
{
	return components;
}
