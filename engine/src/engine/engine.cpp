#include "engine/engine.h"
#include <unordered_set>
#include <ranges>
#include "IImage.h"
#include "components/light.h"
#include "components/mesh_renderer.h"
#include "components/text_mesh.h"
#include "components/text.h"
#include "components/button.h"
#include "debug/log.h"
#include "components/vehicle_controller.h"
#include "components/box_collider.h"
#include "components/sphere_collider.h"
#include "components/rigidbody.h"
#include "components/convex_collider.h"
#include "components/mesh_collider.h"
#include "components/game_camera.h"
#include "resources/resource_manager.h"
#include "resources/texture.h"
#include "scripting/scripting.h"
#include "scripting/script_manager.h"
#include "../../include/scripting/lib_loader.h"
#include "serialization/serializer.h"
#include "sound/sound_emitter.h"
#include "sound/sound_listener.h"
#include "components/material_manager.h"
#include "physics/physics.h"
#include "renderer/widget_renderer.h"
#include "multithreading/async.h"

std::vector<Engine::ModelToLoad> Engine::modelsAwaiting;
bool Engine::nextSceneLoaded = false;
std::string Engine::nextSceneName;

void Engine::CreateContext()
{
	context = new RenderContext();
	context->Create();
	CreateEditorTextures();
	CreateCompReflectionInfo();
	WidgetRenderer::GetInstance().Create(context->GetRHI());
}

void Engine::CreateRenderer(Window* window)
{
	renderer = new Renderer();
	renderer->Create(context);
	renderer->SetupPipelines();
	InputManager::SetCurrentWindow(window);
}

void Engine::Destroy()
{
	context->Destroy();
	renderer->Destroy();

	delete context;
	delete renderer;
}

void Engine::UpdateRenderer(Scene* currentScene)
{
	renderer->DrawFrame(currentScene);

	CheckIfModelsLoaded();

	CheckIfSceneLoaded();
}

unsigned int Engine::GetViewportTexture()
{
	return renderer->GetEditorTexture();
}

unsigned int Engine::GetGameTexture()
{
	return renderer->GetGameTexture();
}

unsigned int Engine::GetEditorTexture(std::string name)
{
	auto it = editorTextures.find(name);
	if (it == editorTextures.end())
	{
		Debug::LogError("Editor texture not found !");
		return 0;
	}
	return it->second->GetID();
}

void Engine::PickObject(Math::Vector2D viewportSize, Math::Vector2D viewportPos)
{
	Math::Vector2D screenMousePos = InputManager::GetMousePos();
	Math::Vector2D viewportMousePos = screenMousePos - viewportPos;
	
	float x = (2.0f * viewportMousePos.x) / viewportSize.x - 1.0f;
	float y = 1.0f - (2.0f * viewportMousePos.y) / viewportSize.y;
	float z = 1.0f;

	// range [-1:1, -1:1, -1:1]
	Math::Vector3D rayNDS = Math::Vector3D(x, y, z);
	// range [-1:1, -1:1, -1:1, -1:1]
	Math::Vector4D rayClip = Math::Vector4D(rayNDS.x, rayNDS.y, -1.0f, 1.0f);

	Scene* currentScene = SceneManager::GetInstance().GetCurrentScene();
	EngineCamera* cam = currentScene->GetEngineCam();
	Math::Matrix4x4 invProjection = cam->GetProjection().Inverse();
	Math::Matrix4x4 invView = cam->GetView().Inverse();
	Math::Vector4D rayEye = invProjection * rayClip;
	rayEye = Math::Vector4D(rayEye.x, rayEye.y, -1.0, 0.0);
	Math::Vector3D rayWorld = invView * rayEye;

	Math::Ray ray
	{
		.origin = cam->GetPosition(),
		.direction = rayWorld.Normalized(),
		.length = cam->GetZFar(),
	};

	Gizmos::color = { 1.f, 1.f, 1.f, 1.f };

	Gizmos::DrawLine(ray.origin, ray.direction * ray.length);

	GameObject* selected = nullptr;
	float closestHit = cam->GetZFar();

	for (GameObject* go : currentScene->GetGameObjects())
	{
		MeshRenderer* mr = go->GetComponent<MeshRenderer>();

		if (!mr)
			continue;

		float aabbDistance;

		if (!Math::Collision::CheckAABBRay(ray, mr->GetAABB(), aabbDistance))
			continue;

		if (aabbDistance > closestHit)
			continue;

		if (go == renderer->GetSelectedGameObject())
			continue;

		for (size_t i = 0; i < mr->GetMesh()->GetIndices().size(); i += 3)
		{
			Math::Vector3D v0 = go->transform.global *
				Math::Vector4D(
					mr->GetMesh()->GetVertices()[mr->GetMesh()->GetIndices()[i]].position,
					1.f);

			Math::Vector3D v1 = go->transform.global *
				Math::Vector4D(
					mr->GetMesh()->GetVertices()[mr->GetMesh()->GetIndices()[i + 1]].position,
					1.f);

			Math::Vector3D v2 = go->transform.global *
				Math::Vector4D(
					mr->GetMesh()->GetVertices()[mr->GetMesh()->GetIndices()[i + 2]].position,
					1.f);

			float triangleDistance;

			if (Math::Collision::CheckTriangleRay(ray, { v0, v1, v2 }, triangleDistance))
			{
				if (triangleDistance < closestHit)
				{
					closestHit = triangleDistance;
					selected = go;
				}
			}
		}
	}

	Math::Vector3D camRight = Math::Vector3D(invView[0], invView[1], invView[2]);
	Math::Vector3D camUp = Math::Vector3D(invView[4], invView[5], invView[6]);

	for (GameObject* go : currentScene->GetGameObjects())
	{
		Light* light = go->GetComponent<Light>();
		if (!light) continue;
		if (go == renderer->GetSelectedGameObject()) continue;

		Math::Vector3D center = light->GetPosition();
		float halfSize = light->GetBillboardScale();

		Math::Vector3D bl = center - camRight * halfSize - camUp * halfSize;
		Math::Vector3D br = center + camRight * halfSize - camUp * halfSize;
		Math::Vector3D tl = center - camRight * halfSize + camUp * halfSize;
		Math::Vector3D tr = center + camRight * halfSize + camUp * halfSize;

		float hitDistance;
		bool hit = false;

		if (Math::Collision::CheckTriangleRay(ray, { bl, br, tr }, hitDistance))
			hit = true;

		if (!hit && Math::Collision::CheckTriangleRay(ray, { bl, tr, tl }, hitDistance))
			hit = true;

		if (hit && hitDistance < closestHit)
		{
			closestHit = hitDistance;
			selected = go;
		}
	}

	renderer->SetSelectedGameObject(selected);
}

void Engine::CreateEditorTextures()
{
	std::vector<Texture*> textures = ResourceManager::GetInstance().LoadAllResourcesOfType<Texture>("internal_assets/icons", GetRHI(), "internal_assets/icons");

	EnigmaRHI::ImageSampler sharedSampler
	{
		.imageType = EnigmaRHI::EImageType::TYPE_2D,
		.minFilter = EnigmaRHI::EFilteringMode::LINEAR_MIPMAP_LINEAR,
		.magFilter = EnigmaRHI::EFilteringMode::LINEAR,
		.wrapU = EnigmaRHI::EWrappingMode::CLAMP_TO_BORDER,
		.wrapV = EnigmaRHI::EWrappingMode::CLAMP_TO_BORDER,
		.pixelType = EnigmaRHI::EDataType::UNSIGNED_BYTE,
		.generateMipMaps = true,
	};

	for (int i = 0; i < textures.size(); i++)
	{
		std::string fileName = textures[i]->GetName();

		EnigmaRHI::IImage* image = context->rhi->InstantiateImage();
		image->Create({ textures[i]->GetWidth(), textures[i]->GetHeight(), 1, textures[i]->GetImageFormat(), sharedSampler, textures[i]->GetImageFormat(), textures[i]->GetData() });
		textures[i]->FreeData();

		editorTextures.insert({ fileName, image });
	}
}


const std::vector<ComponentPropertyDescriptor>* Engine::GetComponentPropertyDescriptors(const std::string& componentTypeName) const
{
	auto componentIt = componentPropertyMap.find(componentTypeName);
	return componentIt != componentPropertyMap.end() ? &componentIt->second : nullptr;
}

bool Engine::GetComponentPropertyValue(IComponent* component, const std::string& propertyPath, ComponentPropertyValue& outValue)
{
	if (!component)
		return false;

	const std::vector<std::string> segments = SplitPropertyPath(propertyPath);
	if (segments.empty())
		return false;

	return GetComponentPropertyValueRecursive(rttr::instance(*component), segments, 0, outValue);
}

bool Engine::SetComponentPropertyValue(IComponent* component, const std::string& propertyPath, const ComponentPropertyValue& value)
{
	if (!component)
		return false;

	const std::vector<std::string> segments = SplitPropertyPath(propertyPath);
	if (segments.empty())
		return false;

	return SetComponentPropertyValueRecursive(rttr::instance(*component), segments, 0, value);
}

void Engine::LaunchGame()
{
	Serializer::GetInstance().SaveBeforePlay(SceneManager::GetInstance().GetCurrentScene());
	ScriptManager::GetInstance().Awake();
	ScriptManager::GetInstance().Start();
	WidgetRenderer::GetInstance().EnableInput();

	isRunning = true;
}

void Engine::StopGame(Timer& timer)
{
	if (!requestStopGame)
		return;

	ScriptManager::GetInstance().End();
	WidgetRenderer::GetInstance().DisableInput();
	Scene* currentScene = SceneManager::GetInstance().GetCurrentScene();
	Serializer::GetInstance().ResetAfterPlay(currentScene);
	currentScene->LoadOnlyCurrentSceneAssets(currentScene, GetRHI());
	currentScene->RecreateModelsLoaded(GetRHI());
	timer.Reset();
	renderer->SetSelectedGameObject(nullptr);
	isRunning = false;
	isPaused = false;
	requestStopGame = false;
}

void Engine::PauseGame()
{
	isPaused = !isPaused;
}

void Engine::RequestHotReload()
{
	requestHotReload = true;
}

void Engine::UnloadScripts()
{
	const std::vector<std::string> registeredScriptNames = Scripting::ScriptRegistry::GetInstance().GetRegisteredNames();
	ScriptManager::GetInstance().UnregisterScripts();
	ClearScriptReflectionData(registeredScriptNames);
	Scripting::ScriptRegistry::GetInstance().DeleteClassRegistry();

	LibLoader::UnloadScriptDLL();
}

void Engine::RequestStopGame()
{
	if (!isRunning)
		return;
	requestStopGame = true;
}

void Engine::LinkDll()
{
	if (LibLoader::LoadScriptDLL())
		Serializer::GetInstance().AttachedScript(SceneManager::GetInstance().GetCurrentScene());
	else
		RequestHotReload();
}

void Engine::HotReload()
{
	if (!requestHotReload || !firstLaunch || isHotReloading)
		return;

	auto AsyncHotReload = []()
		{
			requestHotReload = false;
			isHotReloading = true;

			UnloadScripts();
			LibLoader::WriteInJson();
			LibLoader::CreateCppFromJson();
			if (!LibLoader::CompileScript())
			{
				isHotReloading = false;
				return;
			}

			if (!LibLoader::LoadScriptDLL())
			{
				isHotReloading = false;
				return;
			}

			Serializer::GetInstance().AttachedScript(SceneManager::GetInstance().GetCurrentScene());
			isHotReloading = false;
		};

	Async::GetInstance().CreateTask(AsyncHotReload);
}

void Engine::CachePropertyData(const rttr::type& type)
{
	std::vector<ComponentPropertyDescriptor>& descriptors = componentPropertyMap[type.get_name().to_string()];
	descriptors.clear();
	CollectComponentProperties(type, "", descriptors);
}

void Engine::ClearScriptReflectionData(const std::vector<std::string>& scriptTypeNames)
{
	const std::unordered_set<std::string> scriptTypeNameSet(scriptTypeNames.begin(), scriptTypeNames.end());
	for (const std::string& scriptTypeName : scriptTypeNameSet)
		componentPropertyMap.erase(scriptTypeName);

	const rttr::type scriptBaseType = rttr::type::get<Scripting::IScript>();
	for (auto it = componentsMap.begin(); it != componentsMap.end();)
	{
		rttr::type type = it->first;
		if (!type.is_valid())
		{
			++it;
			continue;
		}

		if (type.is_pointer())
			type = type.get_raw_type();

		const std::string typeName = type.get_name().to_string();
		const bool isRegisteredScript = scriptTypeNameSet.contains(typeName);
		const bool isReflectedScript = type.is_valid() && scriptBaseType.is_valid() && type.is_derived_from(scriptBaseType);

		if (isRegisteredScript || isReflectedScript)
		{
			componentPropertyMap.erase(typeName);
			it = componentsMap.erase(it);
			continue;
		}
		++it;
	}
}

void Engine::AddModelToAwait(std::string modelName, GameObject* go, Scene* scene)
{
	ModelToLoad modelToLoad
	{
		.modelName = modelName,
		.go = go,
		.scene = scene
	};
	modelsAwaiting.push_back(modelToLoad);
}

void Engine::CreateCompReflectionInfo()
{
	componentsMap =
	{
		{ rttr::type::get<Light>(), { "Light", "directional_icon.png", [&] { GetRenderer()->GetSelectedGameObject()->AddComponent<Light>(); } } },
		{ rttr::type::get<GameCamera>(), { "Game Camera", "camera_icon.png", [&] { GetRenderer()->GetSelectedGameObject()->AddComponent<GameCamera>(); } } },
		{ rttr::type::get<MeshRenderer>(), { "Mesh Renderer", "mesh_renderer_icon.png", [&] { GetRenderer()->GetSelectedGameObject()->AddComponent<MeshRenderer>(); } } },
		{ rttr::type::get<Rigidbody>(), { "Rigidbody", "rigidbody_icon.png", [&] { GetRenderer()->GetSelectedGameObject()->AddComponent<Rigidbody>(); } } },
		{ rttr::type::get<BoxCollider>(), { "Box Collider", "box_collider_icon.png", [&] { GetRenderer()->GetSelectedGameObject()->AddComponent<BoxCollider>(); } } },
		{ rttr::type::get<SphereCollider>(), { "Sphere Collider", "sphere_collider_icon.png", [&] { GetRenderer()->GetSelectedGameObject()->AddComponent<SphereCollider>(); } } },
		{ rttr::type::get<MeshCollider>(), { "Mesh Collider", "mesh_collider_icon.png", [&] { GetRenderer()->GetSelectedGameObject()->AddComponent<MeshCollider>(); } } },
		{ rttr::type::get<ConvexCollider>(), { "Convex Collider", "mesh_collider_icon.png", [&] { GetRenderer()->GetSelectedGameObject()->AddComponent<ConvexCollider>(); } } },
		{ rttr::type::get<VehicleController>(), { "Vehicle Controller", "vehicle_controller_icon.png", [&] { GetRenderer()->GetSelectedGameObject()->AddComponent<VehicleController>(); } } },
		{ rttr::type::get<SoundEmitter>(), {"SoundEmitter", "emitter_icon.png", [&] { GetRenderer()->GetSelectedGameObject()->AddComponent<SoundEmitter>(); } } },
		{ rttr::type::get<SoundListener>(), {"SoundListener", "listener_icon.png", [&] { GetRenderer()->GetSelectedGameObject()->AddComponent<SoundListener>(); } } },
		{ rttr::type::get<TextMesh>(), { "Text Mesh", "text_mesh_icon.png", [&] { GetRenderer()->GetSelectedGameObject()->AddComponent<TextMesh>(); } }},
		{ rttr::type::get<Text>(), { "UI Text", "text_icon.png", [&] { GetRenderer()->GetSelectedGameObject()->AddComponent<Text>(); } }},
		{ rttr::type::get<Button>(), { "UI Button", "button_icon.png", [&] { GetRenderer()->GetSelectedGameObject()->AddComponent<Button>(); } }},
	};

	componentPropertyMap.clear();

	//Init componentPropertyMap from the lambda function
	CachePropertyData(rttr::type::get<MeshRenderer>());
	CachePropertyData(rttr::type::get<Light>());
	CachePropertyData(rttr::type::get<GameCamera>());
	CachePropertyData(rttr::type::get<SoundEmitter>());
	CachePropertyData(rttr::type::get<SoundListener>());
	CachePropertyData(rttr::type::get<Rigidbody>());
	CachePropertyData(rttr::type::get<BoxCollider>());
	CachePropertyData(rttr::type::get<SphereCollider>());
	CachePropertyData(rttr::type::get<MeshCollider>());
	CachePropertyData(rttr::type::get<ConvexCollider>());
	CachePropertyData(rttr::type::get<VehicleController>());
	CachePropertyData(rttr::type::get<Transform>());
	CachePropertyData(rttr::type::get<TextMesh>());
	CachePropertyData(rttr::type::get<Text>());
	CachePropertyData(rttr::type::get<Button>());

	//Debug
	size_t cachedPropertyCount = 0;
	for (const auto &properties: componentPropertyMap | std::views::values)
		cachedPropertyCount += properties.size();
}

std::vector<std::string> Engine::SplitPropertyPath(const std::string& path)
{
	std::vector<std::string> segments;
	size_t start = 0;

	while (start <= path.size())
	{
		size_t dot = path.find('.', start);
		if (dot == std::string::npos)
		{
			segments.push_back(path.substr(start));
			break;
		}

		segments.push_back(path.substr(start, dot - start));
		start = dot + 1;
	}

	return segments;
}

bool Engine::TryGetComponentValueFromVariant(const rttr::variant& value, ComponentPropertyValue& outValue)
{
	rttr::type type = value.get_type();
	if (type.is_wrapper())
		type = type.get_wrapped_type();

	if (type == rttr::type::get<float>())
	{
		outValue = value.get_value<float>();
		return true;
	}

	if (type == rttr::type::get<int>())
	{
		outValue = value.get_value<int>();
		return true;
	}

	if (type == rttr::type::get<bool>())
	{
		outValue = value.get_value<bool>();
		return true;
	}

	if (type == rttr::type::get<Math::Vector2D>())
	{
		outValue = value.get_value<Math::Vector2D>();
		return true;
	}

	if (type == rttr::type::get<Math::Vector3D>())
	{
		outValue = value.get_value<Math::Vector3D>();
		return true;
	}

	if (type == rttr::type::get<Math::Vector4D>())
	{
		outValue = value.get_value<Math::Vector4D>();
		return true;
	}

	if (type == rttr::type::get<Material*>())
	{
		outValue = value.get_value<Material*>();
		return true;
	}

	if (type == rttr::type::get<FontTexture*>())
	{
		outValue = value.get_value<FontTexture*>();
		return true;
	}

	if (type == rttr::type::get<GameObject*>())
	{
		outValue = value.get_value<GameObject*>();
		return true;
	}

	if (type == rttr::type::get<std::string>())
	{
		outValue = value.get_value<std::string>();
		return true;
	}
	
	if (type.is_enumeration())
	{
		rttr::variant converted = value;
		if (converted.convert<int>())
		{
			outValue = converted.get_value<int>();
			return true;
		}
	}

	return false;
}

void Engine::CollectComponentProperties(rttr::type type, const std::string& prefix, std::vector<ComponentPropertyDescriptor>& outDescriptors)
{
	if (type.is_wrapper())
		type = type.get_wrapped_type();
	if (type.is_pointer())
		type = type.get_raw_type();

	for (const auto& prop : type.get_properties())
	{
		const std::string propertyName = prop.get_name().to_string();
		const std::string propertyPath = prefix.empty() ? propertyName : prefix + "." + propertyName;

		//Non valide au Hot-Reload
		rttr::variant uiMeta = prop.get_metadata(UI_DATA_METADATA_KEY);
		if (uiMeta.is_valid())
			outDescriptors.push_back({ propertyPath, propertyName, uiMeta.get_value<UIData>(), prop.is_readonly() });
	}
}

bool Engine::GetComponentPropertyValueRecursive(const rttr::instance& current, const std::vector<std::string>& segments, size_t index, ComponentPropertyValue& outValue)
{
	rttr::type currentType = current.get_derived_type();
	if (currentType.is_pointer())
		currentType = currentType.get_raw_type();

	rttr::property prop = currentType.get_property(segments[index]);
	if (!prop.is_valid())
		return false;

	rttr::variant value = prop.get_value(current);
	if (!value.is_valid())
		return false;

	if (index == segments.size() - 1)
		return TryGetComponentValueFromVariant(value, outValue);

	return GetComponentPropertyValueRecursive(rttr::instance(value), segments, index + 1, outValue);
}

bool Engine::SetComponentPropertyValueRecursive(const rttr::instance& current, const std::vector<std::string>& segments, size_t index, const ComponentPropertyValue& value)
{
	rttr::type currentType = current.get_derived_type();
	if (currentType.is_pointer())
		currentType = currentType.get_raw_type();

	rttr::property prop = currentType.get_property(segments[index]);
	if (!prop.is_valid())
		return false;

	if (index == segments.size() - 1)
	{
		if (prop.is_readonly())
			return false;

		return std::visit(
			[&](const auto& typedValue)
			{
				using TValue = std::decay_t<decltype(typedValue)>;

				if constexpr (std::is_same_v<TValue, Material*>)
				{
					return false;
				}
				else
				{
					return prop.set_value(current, typedValue);
				}
			},
			value);
	}

	rttr::variant childValue = prop.get_value(current);
	if (!childValue.is_valid())
		return false;

	if (!SetComponentPropertyValueRecursive(rttr::instance(childValue), segments, index + 1, value))
		return false;

	return prop.set_value(current, childValue);
}

void Engine::CheckIfSceneLoaded()
{
	if (nextSceneLoaded)
	{
		Scene* nextScene = SceneManager::GetInstance().GetScene(nextSceneName);
		nextScene->RecreateModelsLoaded(GetRHI());
		SceneManager::GetInstance().SetCurrentScene(nextScene);
		nextSceneLoaded = false;
		nextSceneName = "";

		LightManager::GetInstance().ClearLights();
		WidgetRenderer::GetInstance().ClearWidgets();

		for (GameObject* gO : SceneManager::GetInstance().GetCurrentScene()->GetGameObjects())
		{
			if (gO->IsUIElement())
			{
				if (Button* w = gO->GetComponent<Button>())
					WidgetRenderer::GetInstance().RegisterWidget(w);
				else if (Text* w = gO->GetComponent<Text>())
					WidgetRenderer::GetInstance().RegisterWidget(w);
				else if (TextMesh* w = gO->GetComponent<TextMesh>())
					WidgetRenderer::GetInstance().RegisterWidget(w);
			}
			else
			{
				if (Light* l = gO->GetComponent<Light>())
					LightManager::GetInstance().AddLight(l);
			}
		}
	}
}

void Engine::CheckIfModelsLoaded()
{
	for (auto it = modelsAwaiting.begin(); it != modelsAwaiting.end(); ++it)
	{
		if (Model* m = ResourceManager::GetInstance().Get<Model>(it->modelName))
		{
			m->CreateGPUResources(GetRHI());
			Model::InstantiateIntoScene(m, it->go, it->scene);
			it = modelsAwaiting.erase(it);

			if (it == modelsAwaiting.end())
			{
				break;
			}
		}
	}
}

void Engine::Save()
{
	Serializer::GetInstance().SerializeScene(SceneManager::GetInstance().GetCurrentScene());
	MaterialManager::GetInstance().SaveAllMaterials();
	SceneManager::GetInstance().SaveScenes();
	Debug::Log("Scene saved !");
}

void Engine::ChangeScene(std::string sceneName)
{
	auto LoadScene = [this, sceneName]()
		{
			Scene* nextScene = SceneManager::GetInstance().GetScene(sceneName);
			nextSceneName = sceneName;

			if (!nextScene->IsLoaded())
			{
				Serializer::GetInstance().DeserializeScene(nextScene);
				nextScene->LoadOnlyCurrentSceneAssets(nextScene, GetRHI());
			}
			nextSceneLoaded = true;
		};

	Async::GetInstance().CreateTask(LoadScene);
}

void Engine::SaveMaterial(MeshRenderer* mr, std::string name)
{
	Material* mat = mr->GetMaterial();
	mat->name = name;
	Material* newMat = MaterialManager::GetInstance().AddMaterial(mat);
	mr->SetMaterial(newMat);
}
