#include "gameobject/gameobject.h"

#include "debug/log.h"
#include "scenes/scene_graph.h"
#include "scripting/iscript.h"
#include "scripting/scripting.h"
#include "components/text.h"
#include "components/text_mesh.h"
#include "serialization/serializer.h"
#include "utilities/shortcut.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<GameObject>("GameObject")
		 .property("uuid", &GameObject::uuid)
		 .property("name", &GameObject::name)
		 .property("isInstancePrefab", &GameObject::isInstancePrefab)
		 .property("prefabPath", &GameObject::prefabPath)
		 .property("transform", &GameObject::transform)
		 .property("components", &GameObject::components)
		 .property("children", &GameObject::children);
}

void GameObject::CopyComponent(IComponent *component)
{
	component->SetParent(this);
	components.push_back(component);
}

IComponent* GameObject::AddComponentType(rttr::type type)
{
	for (IComponent* component : components)
	{
		if (rttr::type::get(*component) == type)
		{
			Debug::LogWarning("Cannot add component already exist");
			return component;
		}
	}

	rttr::variant v = type.create();
	IComponent* component = v.get_value<IComponent*>();

	if (!component)
	{
		return nullptr;
	}

	component->SetParent(this);
	component->Create();
	components.push_back(component);

	return component;
}

IComponent* GameObject::GetComponentType(const rttr::type& type) const
{
	for (IComponent* component : components)
	{
		if (rttr::type::get(*component) == type)
		{
			return component;
		}
	}
	return nullptr;
}

void GameObject::DeleteComponent(IComponent* target)
{
	auto it = std::find(components.begin(), components.end(), target);
	if (it != components.end())
	{
		if (dynamic_cast<Scripting::IScript*>(*it))
		{
			DeleteScript(dynamic_cast<Scripting::IScript*>(*it)->GetScriptName());
			return;
		}
		(*it)->Destroy();
		delete *it;
		components.erase(it);
		return;
	}
	Debug::LogError("Delete component failed");
}

Scripting::IScript* GameObject::AddScript(const std::string& scriptName)
{
	Scripting::IScript* script = Scripting::ScriptRegistry::GetInstance().Create(scriptName);
	if (!script)
	{
		return nullptr;
	}

	script->SetParent(this);
	script->Create();
	components.push_back(script);
	return script;
}

void GameObject::DeleteScript(const std::string& scriptName)
{
	components.erase(std::ranges::remove_if(components, [&](IComponent* component)
	{
		Scripting::IScript* script = dynamic_cast<Scripting::IScript*>(component);
		if (script && script->GetScriptName() == scriptName)
		{
			script->Destroy();
			delete script;
			Debug::LogSuccess("Deleted script " + scriptName + " from GameObject " + name);
			return true;
		}
		return false;
	}).begin(), components.end());
}

void GameObject::SetInstancePrefab(std::string path)
{
	isInstancePrefab = true;
	prefabPath = path;
}

void GameObject::SetName(std::string _name)
{
	name = _name;
}

std::string GameObject::GetName()
{
	return name;
}

void GameObject::Destroy()
{
	for (IComponent* component : components)
	{
		component->Destroy();
		delete component;
	}
	components.clear();

	if (physicalBody)
	{
		physicalBody->Destroy();
		delete physicalBody;
		physicalBody = nullptr;
	}

	while (!children.empty())
	{
		children[0]->Destroy();
	}

	if (parent)
	{
		parent->RemoveChild(this);
		parent = nullptr;
	}
}

void GameObject::SetTransform(Math::Vector3D pos, Math::Quaternion quat, Math::Vector3D scale)
{
	transform.position = pos;
	transform.rotation = quat;
	transform.scale = scale;
}

void GameObject::SetTransform(Math::Vector3D pos, Math::Vector3D quat, Math::Vector3D scale)
{
	transform.position = pos;
	transform.rotation = Math::Quaternion::FromEuler(quat);
	transform.scale = scale;
}

void GameObject::SetParent(GameObject* _parent)
{
	DetachFromParent();
	parent = _parent;
}

void GameObject::AddChild(GameObject* newChild)
{
	if (newChild->IsAncestorOf(this))
	{
		GameObject* oldParent = newChild->parent;

		this->transform.position = transform.worldPosition;
		this->transform.rotation = transform.worldRotation;
		this->transform.scale = transform.worldScale;

		newChild->RemoveChild(this);
		this->parent = nullptr;

		if (oldParent)
			oldParent->AddChild(this);
	}

	children.push_back(newChild);

	newChild->transform.position = newChild->transform.position - this->transform.position;
	newChild->transform.rotation = this->transform.rotation.Inverse() * newChild->transform.rotation;
	newChild->transform.scale = newChild->transform.scale / this->transform.scale;

	newChild->SetParent(this);
}

void GameObject::RemoveChild(GameObject* child)
{
	if (children.empty())
	{
		return;
	}

	auto it = std::find(children.begin(), children.end(), child);
	if (it != children.end())
	{
		children.erase(it);
	}
}

void GameObject::DetachFromParent()
{
	if (parent)
	{
		if(parent->GetChildren().size() > 0)
			parent->RemoveChild(this);
	}
}

void GameObject::RebuildLayer()
{
	std::variant<Text, TextMesh> uiComponents;

	for (IComponent* component : components)
	{
		if (typeid(component) == typeid(uiComponents))
		{
			layer = ELayer::UI;
			return;
		}
	}

	layer = ELayer::OBJECT;
}

bool GameObject::IsInstancePrefab()
{
	return isInstancePrefab;
}

bool GameObject::IsAncestorOf(const GameObject* other) const
{
	const GameObject* current = other->parent;
	while (current)
	{
		if (current == this)
			return true;
		current = current->parent;
	}
	return false;
}

GameObject::GameObject()
{
	transform.gameObject = this;
}
