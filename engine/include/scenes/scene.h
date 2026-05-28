#pragma once

#include "gameobject/gameobject.h"
#include "renderer/engine_camera.h"
#include "components/game_camera.h"
#include "multithreading/async.h"
#include <string>
#include <vector>
#include <rttr/registration_friend.h>
#include "utilities/macro.h"

class Model;

class ENIGMA_API Scene
{
public:
	GameObject* AddGameObject(std::string name);
	void DeleteGameObject(GameObject* gameObject);
	void LoadOnlyCurrentSceneAssets(Scene* scene, EnigmaRHI::IRenderInterface* rhi);
	std::vector<GameObject*>& GetGameObjects();
	GameObject* GetGameObject(int _index) const { return gameObjects[_index]; }
	GameObject* GetGameObject(const std::string& name) const;
	EngineCamera* GetEngineCam() const { return camera; };
	GameObject* GetGameCam() const { return gameCam; };
	void SetGameCam(GameObject* newGameCam);
	void SetGameCam(const GameCamera* newGameCam);
	void Destroy();
	void Clean();
	void SetIndex(int index);
	void SetName(const std::string& name);
	void PostBuild();
	std::string GetName();
	std::string GetSkyboxPath() const { return skyboxPath; };
	void SetSkyboxPath(const std::string& newPath) { skyboxPath = newPath; };
	bool IsLoaded();
	void SetLoaded(bool isLoaded);
	void RecreateModelsLoaded(EnigmaRHI::IRenderInterface* rhi);

private:
	RTTR_REGISTRATION_FRIEND
	std::vector<GameObject*> gameObjects;
	std::vector<Model*> modelsAwaiting;

	int index = 0;
	std::string name;
	bool isLoaded = false;
	EngineCamera* camera = nullptr;
	GameObject* gameCam = nullptr;
	GameObject* world = nullptr;
	std::mutex mut;

	std::string skyboxPath = "shaders/sky.hdr";

	Scene();
	~Scene() = default;
	
	friend class SceneManager;
};