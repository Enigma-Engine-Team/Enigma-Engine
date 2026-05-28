#include "gameobject/gameobject.h"

#include "debug/log.h"
#include "scenes/scene_graph.h"
#include "scripting/iscript.h"
#include "scripting/scripting.h"
#include "components/text.h"
#include "components/text_mesh.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<GameObject>("GameObject")
		 .property("name", &GameObject::name)
		 .property("isInstancePrefab", &GameObject::isInstancePrefab)
		 .property("prefabPath", &GameObject::prefabPath)
		 .property("transform", &GameObject::transform)
		 .property("components", &GameObject::components)
		 .property("children", &GameObject::children);
}

void GameObject::SetName(std::string _name)
{
	name = _name;
}

std::string GameObject::GetName()
{
	return name;
}

void GameObject::SetTransform(Math::Vector3D pos, Math::Quaternion quat, Math::Vector3D scale)
{
	transform.SetPosition(pos);
	transform.SetRotation(quat);
	transform.SetScale(scale);
}

void GameObject::SetTransform(Math::Vector3D pos, Math::Vector3D quat, Math::Vector3D scale)
{
	transform.SetPosition(pos);
	transform.SetRotation(Math::Quaternion::FromEuler(quat));
	transform.SetScale(scale);
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

		this->transform.SetPosition(transform.GetWorldPosition());
		this->transform.SetRotation(transform.GetWorldRotation());
		this->transform.SetScale(transform.GetWorldScale());

		newChild->RemoveChild(this);
		this->parent = nullptr;

		if (oldParent)
			oldParent->AddChild(this);
	}

	children.push_back(newChild);

	newChild->transform.SetPosition(newChild->transform.GetPosition() - this->transform.GetPosition());
	newChild->transform.SetRotation(this->transform.GetRotation().Inverse() * newChild->transform.GetRotation());
	newChild->transform.SetScale(newChild->transform.GetScale() / this->transform.GetScale());

	newChild->SetParent(this);
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

GameObject::GameObject()
{
	transform.gameObject = this;
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

bool GameObject::IsInstancePrefab()
{
	return isInstancePrefab;
}

void GameObject::SetInstancePrefab(std::string path)
{
	isInstancePrefab = true;
	prefabPath = path;
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
