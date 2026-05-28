#include "scenes/scene_manager.h"
#include <rttr/registration.h>
#include <fstream>

RTTR_REGISTRATION
{
	rttr::registration::class_<SceneManager>("SceneManager")
		 .property("scenes", &SceneManager::scenes);
}

SceneManager& SceneManager::GetInstance()
{
	static SceneManager sceneManager;
	return sceneManager;
}

void SceneManager::CreateScenes(std::string _path)
{
	path = _path;
	if (!std::filesystem::exists(path))
	{
		std::ofstream file(path);
		if (file.is_open())
		{
			file << "Scene0" << std::endl << "0";
			file.close();
		}
	}
	std::ifstream file(path);
	std::string line;
	int currentSceneIndex = 0;
	while (std::getline(file, line))
	{
		if (!line.empty() && std::all_of(line.begin(), line.end(), ::isdigit))
		{
			currentSceneIndex = std::stoi(line);
			break;
		}

		AddScene();
		scenes.back()->SetName(line);
	}

	if (scenes.size() > 0)
	{
		SetCurrentScene(scenes[currentSceneIndex]);
	}
}

void SceneManager::SaveScenes()
{
	std::ofstream file(path);
	if (file.is_open())
	{
		for (Scene* scene : scenes)
		{
			file << scene->GetName() << std::endl;
		}
		file << currentScene->index;
	}
}

Scene* SceneManager::AddScene()
{
	Scene* newScene = new Scene();
	newScene->SetIndex(static_cast<int>(scenes.size()));
	scenes.push_back(newScene);
	return newScene;
}

Scene* SceneManager::GetScene(int index)
{
	return scenes.size() > index ? scenes[index] : nullptr;
}

Scene* SceneManager::GetScene(std::string name)
{
	for (Scene* scene : scenes)
	{
		if (scene->GetName() == name)
		{
			return scene;
		}
	}
	return nullptr;
}

void SceneManager::SetCurrentScene(Scene* scene)
{
	currentScene = scene;
}

void SceneManager::SetCurrentScene(int index)
{
	currentScene = scenes[index];
}

void SceneManager::SetCurrentScene(std::string name)
{
	for (Scene* scene : scenes)
	{
		if (scene->GetName() == name)
		{
			currentScene = scene;
		}
	}
}

void SceneManager::DeleteScene(int index)
{
	delete scenes[index];
	scenes.erase(scenes.begin() + index);
}

void SceneManager::Destroy()
{
	for (Scene* scene : scenes)
	{
		scene->Destroy();
		delete scene;
	}
	scenes.clear();
}