#include "scenes/scene.h"
#include "serialization/serializer.h"
#include "components/mesh_renderer.h"
#include "resources/resource_manager.h"
#include <rttr/registration.h>
#include <chrono>

#include "components/convex_collider.h"
#include "components/mesh_collider.h"
#include "components/vehicle_controller.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<Scene>("Scene")
		 .property("index", &Scene::index)
		 .property("skyboxPath", &Scene::skyboxPath)
		 .property("gameObjects", &Scene::gameObjects);
}

Scene::Scene()
{
	camera = new EngineCamera();
	camera->Create();

	world = new GameObject();
	world->SetName("world");
	world->transform.position = { 0.f, 0.f, 0.f };
	world->transform.rotation = Math::Quaternion::FromEuler({ 0.f, 0.f, 0.f });
	world->transform.scale = { 1.f, 1.f, 1.f };

	gameObjects.push_back(world);
}

void Scene::RecreateModelsLoaded(EnigmaRHI::IRenderInterface* rhi)
{
	for (Model* model : modelsAwaiting)
	{
		model->CreateGPUResources(rhi);
	}
	for (GameObject* go : gameObjects)
	{
		if (MeshRenderer* renderer = go->GetComponent<MeshRenderer>())
			renderer->Recreate(rhi);
	}
	for (GameObject* go : gameObjects)
	{
		if (VehicleController* vehicle = go->GetComponent<VehicleController>())
			vehicle->Rebuild();
	}

	modelsAwaiting.clear();
}

GameObject* Scene::AddGameObject(std::string _name)
{
	GameObject* newGameObject = new GameObject();
	newGameObject->SetName(_name);
	gameObjects.push_back(newGameObject);
	world->AddChild(newGameObject);
	return newGameObject;
}

void Scene::DeleteGameObject(GameObject* gameObject)
{
	auto it = std::find(gameObjects.begin(), gameObjects.end(), gameObject);
	if (it != gameObjects.end())
	{
		(*it)->Destroy();
		delete *it;
		*it = nullptr;

		if (gameObject == gameCam)
		{
			gameCam = nullptr;
		}

		if (gameObjects.back() != *it)
		{
			*it = gameObjects.back();
		}

		gameObjects.pop_back();
	}
}

void Scene::LoadOnlyCurrentSceneAssets(Scene* scene, EnigmaRHI::IRenderInterface* rhi)
{
	std::vector<std::string> modelNames;

	for (GameObject* go : gameObjects)
	{
		if (go->GetComponent<MeshRenderer>())
		{
			std::string modelToLoad = go->GetComponent<MeshRenderer>()->modelName;
			if (ResourceManager::GetInstance().Get<Model>(modelToLoad) == nullptr &&
				std::find(modelNames.begin(), modelNames.end(), modelToLoad) == modelNames.end())
			{
				modelNames.push_back(modelToLoad);
			}
		}
	}

	std::vector<int> indices;
	auto start = std::chrono::high_resolution_clock::now();

	for (std::string modelName : modelNames)
	{
		auto CreateModel = [this, modelName, rhi]()
			{
				Model* model = ResourceManager::GetInstance().Create<Model>(modelName, rhi);
				std::lock_guard<std::mutex> lock(mut);
				modelsAwaiting.push_back(model);
			};
		int i = Async::GetInstance().CreateTask(CreateModel);
		indices.push_back(i);
	}

	for (int i : indices)
	{
		Async::GetInstance().WaitTask(i);
	}

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	Debug::LogSuccess("Loading scene took " + std::to_string(duration.count()) + "ms");
}

std::vector<GameObject*>& Scene::GetGameObjects()
{
	return gameObjects;
}

GameObject* Scene::GetGameObject(const std::string& name) const
{
	for (GameObject* gameObject : gameObjects)
	{
		if (gameObject->GetName() == name)
		{
			return gameObject;
		}
	}
	return nullptr;
}

void Scene::SetGameCam(GameObject* newGameCam)
{
	if (newGameCam->GetComponent<GameCamera>())
		gameCam = newGameCam;
}

void Scene::SetGameCam(const GameCamera* newGameCam)
{
	gameCam = newGameCam->gameObject;
}


void Scene::Destroy()
{
	for (GameObject* gameObject : gameObjects)
	{
		gameObject->Destroy();
		delete gameObject;
	}
	gameObjects.clear();
}

void Scene::Clean()
{
	int i = 0;
	for (GameObject* gameObject : gameObjects)
	{
		if (i == 0)
		{
			++i;
			continue;
		}

		gameObject->Destroy();
		delete gameObject;
		++i;
	}

	gameObjects.erase(gameObjects.begin() + 1, gameObjects.end());

	gameCam = nullptr;
}

void Scene::SetIndex(int _index)
{
	index = _index;
	name = "Scene" + std::to_string(index);
}

void Scene::SetName(const std::string& _name)
{
	name = _name;
}

void Scene::PostBuild()
{
	for (GameObject* go : gameObjects)
	{
		if (VehicleController* vehicle = go->GetComponent<VehicleController>())
			vehicle->Rebuild();
	}
}

std::string Scene::GetName()
{
	return name;
}

bool Scene::IsLoaded()
{
	return isLoaded;
}

void Scene::SetLoaded(bool _isLoaded)
{
	isLoaded = _isLoaded;
}
