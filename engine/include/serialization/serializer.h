#pragma once

#include "scenes/scene_manager.h"
#include "components/material.h"
#include <rttr/registration.h>
#include <nlohmann/json.hpp>
#include "../utilities/macro.h"

class MeshRenderer;

class ENIGMA_API Serializer
{
public:
	static Serializer& GetInstance();

	void SerializeScene(Scene* scene);
	void DeserializeScene(Scene* scene);

	void SaveBeforePlay(Scene* scene);
	void ResetAfterPlay(Scene* scene);

	void WritePrefab(GameObject* gameObject);
	void ReadPrefab(Scene* scene, std::string name);

	void SerializeMaterial(Material* mat);
	void DeserializeMaterial(Material* mat);

	void AttachedScript(Scene* scene);

private:
	Serializer() = default;
	~Serializer() = default;

	void RecursiveSerialize(const rttr::instance& instance, nlohmann::json& j);
	void RecursiveDeserialize(Scene* scene, nlohmann::json& j, GameObject* go);
	void SetScriptElement(nlohmann::json& j, const std::string& scriptName, const std::string& goName);
	void SetScriptElement(const rttr::instance& instance, const std::string& goName);
	void PushScriptElement(const std::string& goName, const nlohmann::json& scriptData);
	void AttachScriptData(GameObject* go, const nlohmann::json& scriptData, const nlohmann::json& scriptRoot);
	void RecursiveSetProperties(rttr::instance obj, const nlohmann::json& j);

	bool DeleteFileInfos(const std::string &folder, const std::string &file);
	bool ReadFromFile(std::string path, nlohmann::json& datas);
	bool WriteToFile(std::string folder, std::string file, nlohmann::json& datas, bool override = true);

	nlohmann::json script;
	Scene* currentSceneContext = nullptr;
};
