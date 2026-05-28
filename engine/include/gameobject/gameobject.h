#pragma once

#include <concepts>
#include <vector>
#include <string>
#include <rttr/registration.h>
#include <rttr/registration_friend.h>
#include "components/icomponent.h"
#include "physics/physicalBody.h"
#include "transform.h"
#include "../utilities/macro.h"

enum class ELayer
{
	OBJECT,
	UI
};

namespace Scripting {
	class IScript;
}

class ENIGMA_API GameObject
{
public:
	template<typename T>
	requires std::derived_from<T, IComponent>
	T* AddComponent();

	IComponent* AddComponentType(rttr::type type);

	template<typename T>
	requires std::derived_from<T, IComponent>
	T* GetComponent();

	template<typename T>
	requires std::derived_from<T, IComponent>
	T* GetComponentInChildren();

	IComponent* GetComponentType(const rttr::type &type) const;

	std::vector<IComponent*> GetComponents();

	template<typename T>
	requires std::derived_from<T, IComponent>
	void DeleteComponent();

	void DeleteComponent(IComponent* target);

	Scripting::IScript* AddScript(const std::string& scriptName);
	void DeleteScript(const std::string& scriptName);
	
	void SetInstancePrefab(std::string path);
	void SetName(std::string name);
	std::string GetName();

	void Destroy();
	void SetTransform(Math::Vector3D pos, Math::Quaternion quat, Math::Vector3D scale);
	void SetTransform(Math::Vector3D pos, Math::Vector3D quat, Math::Vector3D scale);
	Transform transform;
	Physicalbody* physicalBody = nullptr;

	GameObject* GetChild(int i) const { return children[i]; }
	std::vector<GameObject*> GetChildren() { return children; }
	GameObject* GetParent() const { return parent; }
	void SetParent(GameObject* _parent);
	void AddChild(GameObject* child);
	void RemoveChild(GameObject* child);
	void DetachFromParent();
	bool IsUIElement() const { return layer == ELayer::UI ? true : false; };
	void SetLayer(ELayer newLayer) { layer = newLayer; };
	void RebuildLayer();
	bool IsInstancePrefab();
	bool GetPrefabPath();
	bool IsAncestorOf(const GameObject* other) const;

private:
	RTTR_REGISTRATION_FRIEND
	std::string name;
	std::vector<IComponent*> components;
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
	ELayer layer = ELayer::OBJECT;
	bool isInstancePrefab = false;
	std::string prefabPath = "";

	GameObject();
	~GameObject() = default;
	friend class Scene;
};

#include "gameobject.inl"