#include "scripting/script_manager.h"

#include <algorithm>
#include <filesystem>
#include <fstream>

#include "debug/log.h"
#include "physics/physicalBody.h"
#include "scripting/collision_script_manager.h"
#include "scripting/scripting.h"

ScriptManager& ScriptManager::GetInstance()
{
	static ScriptManager scriptManager;
	return scriptManager;
}

void ScriptManager::Awake()
{
	for (auto script : collisionEnter)
		if (const auto pb = script->gameObject->physicalBody)
			pb->collisionManager->collisionEnter.push_back(script);
	for (auto script : collisionStay)
		if (const auto pb = script->gameObject->physicalBody)
			pb->collisionManager->collisionStay.push_back(script);
	for (auto script : collisionExit)
		if (const auto pb = script->gameObject->physicalBody)
			pb->collisionManager->collisionExit.push_back(script);

	for (Scripting::IScript* script : awake)
	{
		script->Awake();
	}
}

void ScriptManager::Start()
{
	for (Scripting::IScript* script : start)
	{
		script->Start();
	}
}

void ScriptManager::Update(float deltaTime)
{
	for (Scripting::IScript* script : update)
	{
		script->Update(deltaTime);
	}
}

void ScriptManager::FixedUpdate(float fixedDeltaTime)
{
	for (Scripting::IScript* script : fixedUpdate)
	{
		script->FixedUpdate(fixedDeltaTime);
	}
}

void ScriptManager::LateUpdate(float deltaTime)
{
	for (Scripting::IScript* script : lateUpdate)
	{
		script->LateUpdate(deltaTime);
	}
}

void ScriptManager::End()
{
	for (Scripting::IScript* script : end)
	{
		script->End();
	}
}

void ScriptManager::UnregisterScript(Scripting::IScript* script)
{
	std::erase(awake, script);
	std::erase(start, script);
	std::erase(update, script);
	std::erase(fixedUpdate, script);
	std::erase(lateUpdate, script);
	std::erase(end, script);
	std::erase(collisionEnter, script);
	std::erase(collisionStay, script);
	std::erase(collisionExit, script);
	if (Physicalbody* pb = script->gameObject->physicalBody)
	{
		std::erase(pb->collisionManager->collisionEnter, script);
		std::erase(pb->collisionManager->collisionStay, script);
		std::erase(pb->collisionManager->collisionExit, script);
	}
	std::erase(scripts, script);
}

void ScriptManager::UnregisterScripts()
{
	const std::vector<Scripting::IScript*> scriptsSnapshot = scripts;

	for (Scripting::IScript* script : scriptsSnapshot)
	{
		if (!script || !script->gameObject)
			continue;

		script->gameObject->DeleteScript(script->GetScriptName());
	}

	scripts.clear();
	awake.clear();
	start.clear();
	update.clear();
	lateUpdate.clear();
	fixedUpdate.clear();
	lateUpdate.clear();
	collisionEnter.clear();
	collisionStay.clear();
	collisionExit.clear();
}

bool ScriptManager::AddScriptFromEditor(const std::string& scriptName) const
{
	const std::filesystem::path scriptingDirectory = "scripting";
	const std::filesystem::path headerPath = scriptingDirectory / (scriptName + ".h");
	const std::filesystem::path sourcePath = scriptingDirectory / (scriptName + ".cpp");

	if (!std::filesystem::exists(scriptingDirectory))
		std::filesystem::create_directories(scriptingDirectory);

	if (std::filesystem::exists(headerPath) || std::filesystem::exists(sourcePath))
	{
		Debug::LogWarning("Script template already exists: " + scriptName);
		return false;
	}

	std::ofstream headerFile(headerPath);
	std::ofstream sourceFile(sourcePath);
	if (!headerFile.is_open() || !sourceFile.is_open())
	{
		Debug::LogError("Unable to create script template: " + scriptName);
		return false;
	}

	headerFile << "#pragma once\n\n";
	headerFile << "#include \"iscript.h\"\n\n";
	headerFile << "class " << scriptName << " : public Scripting::IScript\n";
	headerFile << "{\n";
	headerFile << "public:\n";
	headerFile << "\tvoid Awake() override;\n";
	headerFile << "\tvoid Start() override;\n";
	headerFile << "\tvoid Update() override;\n";
	headerFile << "};\n";

	sourceFile << "#include \"" << scriptName << ".h\"\n";
	sourceFile << "#include \"scripting.h\"\n\n";
	sourceFile << "REGISTER_SCRIPT(" << scriptName << ");\n\n";
	sourceFile << "void " << scriptName << "::Awake()\n";
	sourceFile << "{\n";
	sourceFile << "}\n\n";
	sourceFile << "void " << scriptName << "::Start()\n";
	sourceFile << "{\n";
	sourceFile << "}\n\n";
	sourceFile << "void " << scriptName << "::Update()\n";
	sourceFile << "{\n";
	sourceFile << "}\n";

	Debug::LogSuccess("Created script template: " + scriptName);
	return true;
}

bool ScriptManager::IsRegistered(const Scripting::IScript *script) const
{
	return std::find(scripts.begin(), scripts.end(), script) != scripts.end();
}