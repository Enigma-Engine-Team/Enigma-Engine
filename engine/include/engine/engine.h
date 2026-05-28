#pragma once

#include "components/icomponent.h"
#include "emath/emath.h"
#include "renderer/render_context.h"
#include "renderer/renderer.h"
#include "window/input_manager.h"
#include "utilities/macro.h"
#include <functional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "timer/timer.h"

class Material;
class FontTexture;
class MeshRenderer;

using ComponentPropertyValue = std::variant<float, int, bool, Math::Vector2D, Math::Vector3D, Math::Vector4D, std::string, Material*, FontTexture*, GameObject*>;

struct ComponentPropertyDescriptor
{
	std::string path;
	std::string name;
	UIData uiData;
	bool readOnly = false;
};

enum EBuildMode
{
	EDITOR,
	GAME
};

class ENIGMA_API Engine
{
public:
	struct ComponentData
	{
		std::string nameToDisplay;
		std::string iconName;
		std::function<void()> func;
	};

	Engine() = default;
	~Engine() = default;

	void CreateContext();
	void CreateRenderer(Window* window);
	void Destroy();

	//Renderer Part
	void UpdateRenderer(Scene* currentScene);
	unsigned int GetViewportTexture();
	unsigned int GetGameTexture();
	static unsigned int GetEditorTexture(std::string name);

	static Renderer* GetRenderer() { return renderer; }
	EnigmaRHI::ERenderAPI GetGraphicsAPI() { return context->GetRHI()->GetGraphicsAPI(); }
	static EnigmaRHI::IRenderInterface* GetRHI() { return context->GetRHI(); }
	void SetViewportSize(float width, float height) { renderer->SetViewportSize(width, height); }
	void SetGizmosActive(bool activated) { renderer->GetGizmoRenderer().SetGizmosActive(activated); };
	bool IsGizmosActive() { return renderer->GetGizmoRenderer().IsGizmosActive(); };
	bool DisplayAABB() { return renderer->GetGizmoRenderer().DisplayAABB(); };
	void SetAABBDisplayActive(bool activated) { renderer->GetGizmoRenderer().SetDisplayAABBActive(activated); };

	void PickObject(Math::Vector2D viewportSize, Math::Vector2D viewportPos);

	void CreateEditorTextures();

	//Scripting Part
	const std::vector<ComponentPropertyDescriptor>* GetComponentPropertyDescriptors(const std::string& componentTypeName) const;
	bool GetComponentPropertyValue(IComponent* component, const std::string& propertyPath, ComponentPropertyValue& outValue);
	bool SetComponentPropertyValue(IComponent* component, const std::string& propertyPath, const ComponentPropertyValue& value);

	static void LaunchGame();
	void StopGame(Timer& timer);
	static void PauseGame();
	static void RequestStopGame();
	static void LinkDll();
	static void RequestHotReload();
	static void UnloadScripts();
	static void HotReload();
	static bool IsPaused() { return isPaused; }
	static bool IsRunning() { return isRunning; }

	void Save();
	void ChangeScene(std::string sceneName);
	void SaveMaterial(MeshRenderer* mr, std::string name);

	static bool IsReloadingScript() { return isHotReloading; }

	inline static std::unordered_map<rttr::type, ComponentData> componentsMap;
	static void CachePropertyData(const rttr::type& type);

	static void SetBuildMode(EBuildMode currentBuildMode) { buildMode = currentBuildMode; }
	static EBuildMode GetBuildMode() { return buildMode; }

	static bool IsInGameBuild() { return buildMode == EBuildMode::GAME; }

	static void AddModelToAwait(std::string modelName, GameObject* go, Scene* scene);

private:
	void CreateCompReflectionInfo();

	std::vector<std::string> SplitPropertyPath(const std::string& path);
	bool TryGetComponentValueFromVariant(const rttr::variant& value, ComponentPropertyValue& outValue);
	static void CollectComponentProperties(rttr::type type, const std::string& prefix, std::vector<ComponentPropertyDescriptor>& outDescriptors);
	static void ClearScriptReflectionData(const std::vector<std::string>& scriptTypeNames);
	bool GetComponentPropertyValueRecursive(const rttr::instance& current, const std::vector<std::string>& segments, size_t index, ComponentPropertyValue& outValue);
	bool SetComponentPropertyValueRecursive(const rttr::instance& current, const std::vector<std::string>& segments, size_t index, const ComponentPropertyValue& value);
	void CheckIfModelsLoaded();
	void CheckIfSceneLoaded();

	inline static RenderContext* context = nullptr;
	inline static Renderer* renderer = nullptr;
	inline static std::unordered_map<std::string, std::vector<ComponentPropertyDescriptor>> componentPropertyMap;
	inline static std::unordered_map<std::string, EnigmaRHI::IImage*> editorTextures;
	
	inline static bool isRunning = false;
	inline static bool isPaused = false;
	inline static bool firstLaunch = true;
	inline static bool isHotReloading = false;
	inline static bool requestHotReload = false;
	inline static bool requestStopGame = false;
	inline static EBuildMode buildMode = EBuildMode::EDITOR;

	struct ModelToLoad
	{
		std::string modelName;
		GameObject* go = nullptr;
		Scene* scene = nullptr;
	};
	static std::vector<ModelToLoad> modelsAwaiting;

	static bool nextSceneLoaded;
	static std::string nextSceneName;
};