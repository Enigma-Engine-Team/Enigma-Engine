#pragma once

#include "scene.h"
#include <vector>
#include <rttr/registration_friend.h>
#include "../utilities/macro.h"

class ENIGMA_API SceneManager
{
public:
	static SceneManager& GetInstance();

	void CreateScenes(std::string path);
	void SaveScenes();
	Scene* AddScene();
	Scene* GetScene(int index);
	Scene* GetScene(std::string name);
	Scene* GetCurrentScene() { return currentScene; };
	void SetCurrentScene(Scene* scene);
	void SetCurrentScene(int index);
	void SetCurrentScene(std::string name);
	void DeleteScene(int index);
	void Destroy();

private:
	RTTR_REGISTRATION_FRIEND
	std::vector<Scene*> scenes;
	Scene* currentScene = nullptr;
	std::string path;

	SceneManager() = default;
	~SceneManager() = default;
};