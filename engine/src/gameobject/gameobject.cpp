#include "gameobject/gameobject.h"

#include "debug/log.h"
#include "scenes/scene_graph.h"
#include "scripting/iscript.h"
#include "scripting/scripting.h"
#include "components/text.h"
#include "components/text_mesh.h"
#include "serialization/serializer.h"
#include "utilities/shortcut.h"
#include "window/input.h"
#include "window/input_manager.h"

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

	DeleteCompAction(component);

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

void GameObject::DeleteComponent(const IComponent* target)
{
	RegisterCompAction(target);
	DestroyComponent(target);
}

void GameObject::DestroyComponent(const IComponent *target)
{
	const auto it = std::ranges::find(components, target);
	if (it == components.end())
		return;

	(*it)->Destroy();
	delete *it;
	components.erase(it);
}

std::vector<Scripting::IScript*> GameObject::GetScripts() const
{
	std::vector<Scripting::IScript*> scripts;
	for (IComponent* component : components)
	{
		if (Scripting::IScript* script = dynamic_cast<Scripting::IScript*>(component))
		{
			scripts.push_back(script);
		}
	}
	return scripts;
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

void GameObject::DeleteScripts()
{
	for (int i = components.size() - 1; i >= 0; i--)
	{
		if (Scripting::IScript* script = dynamic_cast<Scripting::IScript*>(components[i]))
		{
			script->Destroy();
			delete script;
			components.erase(components.begin() + i);
		}
	}
}

void GameObject::SetInstancePrefab(std::string path)
{
	isInstancePrefab = true;
	prefabPath = path;
}

void GameObject::SetName(const std::string& _name)
{
	if (_name == "world")
	{
		name = _name;
		return;
	}

	int index = 0;
	std::string resultName = _name;
	std::string baseName = GetBaseName(_name);

	while(SceneManager::GetInstance().GetCurrentScene()->GetGameObject(resultName))
	{
		index++;
		resultName = baseName + " (" + std::to_string(index) + ")";
	}

	name = resultName;
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
	if (InputManager::GetMouseButton(MOUSE_BUTTON_LEFT) ||
	   (InputManager::GetKeyDown(KEY_LEFT_CONTROL) && (InputManager::GetKeyDown(KEY_Z) || InputManager::GetKeyDown(KEY_Y))))
	{
		Math::Vector3D oldPos = transform.position;
		Math::Vector3D oldRot = transform.rotation.ToEuler();
		Math::Vector3D oldScale = transform.scale;

		Action action("SetTransform", {}, uuid);
		action.action = [action, oldPos, oldRot, oldScale]() {
			GameObject* go = SceneManager::GetInstance().GetCurrentScene()->GetGameObject(action.goID);
			go->SetTransform(oldPos, oldRot, oldScale);
		};
		Shortcut::AddAction(action);
	}

	transform.position = pos;
	transform.rotation = Math::Quaternion::FromEuler(quat);
	transform.scale = scale;
}

void GameObject::SetParent(GameObject* _parent)
{
	Action action("SetParent", {}, uuid);
	Enigma::UUID oldParentUUID = parent ? parent->GetUUID() : Enigma::UUID();
	action.action = [action, oldParentUUID]()
	{
		GameObject* go = SceneManager::GetInstance().GetCurrentScene()->GetGameObject(action.goID);
		if (GameObject* oldParent = SceneManager::GetInstance().GetCurrentScene()->GetGameObject(oldParentUUID))
			oldParent->AddChild(go);
		else
			SceneManager::GetInstance().GetCurrentScene()->GetGameObject(0)->AddChild(go);
	};
	Shortcut::AddAction(action);

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

	Transform worldTransform = newChild->transform;

	if (GameObject* oldParent = newChild->parent)
	{
		worldTransform.position = oldParent->transform.worldPosition + worldTransform.position * oldParent->transform.scale;
		worldTransform.rotation = oldParent->transform.worldRotation * worldTransform.rotation;
		worldTransform.scale = oldParent->transform.worldScale * worldTransform.scale;
	}

	newChild->transform.position = (worldTransform.worldPosition - this->transform.position) / this->transform.scale;
	newChild->transform.rotation = this->transform.worldRotation.Inverse() * worldTransform.rotation;
	newChild->transform.scale = worldTransform.worldScale / this->transform.scale;

	newChild->SetParent(this);
	children.push_back(newChild);
}

void GameObject::RemoveChild(GameObject* child)
{
	if (children.empty())
		return;

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

void GameObject::RegisterCompAction(const IComponent* component) const
{
	Action action("Restore Component", {}, uuid);
	action.snapshot = nlohmann::json::object();
	Serializer::GetInstance().RecursiveSerialize(component, action.snapshot);
	action.action = [action]() {
		GameObject* go = SceneManager::GetInstance().GetCurrentScene()->GetGameObject(action.goID);
		Serializer::GetInstance().DeserializeComponent(go, action.snapshot);
	};
	Shortcut::AddAction(action);
}

void GameObject::Duplicate()
{
	GameObject* go = this;
	nlohmann::json j;
	if (!parent)
		parent = SceneManager::GetInstance().GetCurrentScene()->GetGameObject(0);
	Serializer::GetInstance().RecursiveSerialize(go, j);
	Serializer::GetInstance().RecursiveDeserialize(SceneManager::GetInstance().GetCurrentScene(), j, parent);

	// Ne marche pas pour la duplication de variable, obligé de serializer le script pour récupérer les variables
	// + Faire en sorte de supprimer les anciennes dll de script pour que les nouvelles soient prises en compte
}

void GameObject::DeleteCompAction(IComponent *component)
{
	Action action("Delete Component", {}, uuid);
	action.action = [this, component]() {
		DeleteComponent(component);
	};
	Shortcut::AddAction(action);
}

std::string GameObject::GetBaseName(const std::string& _name)
{
	size_t open = _name.rfind('(');

	if (open == std::string::npos)
		return _name;

	if (_name.back() != ')')
		return _name;

	std::string number = _name.substr(
		open + 1,
		_name.size() - open - 2
	);

	if (number.empty() || !std::all_of(number.begin(), number.end(), ::isdigit))
		return _name;


	if (open == 0 || _name[open - 1] != ' ')
		return _name;

	return _name.substr(0, open - 1);
}

GameObject::GameObject()
{
	transform.gameObject = this;
}
