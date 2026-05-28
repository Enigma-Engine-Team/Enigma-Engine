#pragma once

#include <string>
#include <vector>
#include "iscript.h"
#include "../utilities/macro.h"

class ENIGMA_API ScriptManager
{
public:
	static ScriptManager& GetInstance();

	void Awake();
	void Start();
	void Update(float deltaTime);
	void FixedUpdate(float fixedDeltaTime);
	void LateUpdate(float deltaTime);
	void End();

	template<typename T>
	requires std::derived_from<T, Scripting::IScript>
	void RegisterScript(T* instance);
	void UnregisterScript(Scripting::IScript* script);
	void UnregisterScripts();

	bool AddScriptFromEditor(const std::string& scriptName = "NewScript") const;

private:
	struct AttachedScript
	{
		std::string scriptName;
		GameObject* gameObject;
	};

	bool IsRegistered(const Scripting::IScript* script) const;

	std::vector<Scripting::IScript*> scripts;
	std::vector<Scripting::IScript*> awake;
	std::vector<Scripting::IScript*> start;
	std::vector<Scripting::IScript*> update;
	std::vector<Scripting::IScript*> fixedUpdate;
	std::vector<Scripting::IScript*> lateUpdate;
	std::vector<Scripting::IScript*> end;

	//Temp duplication while I don't see other solution
	std::vector<Scripting::IScript*> collisionEnter;
	std::vector<Scripting::IScript*> collisionStay;
	std::vector<Scripting::IScript*> collisionExit;

	ScriptManager() = default;
	~ScriptManager() = default;
};

#include "script_manager.inl"