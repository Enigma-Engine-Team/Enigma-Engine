#include "network/network.h"
#include "context/editor_ui.h"
#include "scenes/scene_manager.h"
#include "timer/time.h"
#include "engine/engine.h"
#include "scripting/script_manager.h"
#include "physics/physics.h"
#include "serialization/serializer.h"
#include "sound/sound_manager.h"
#include "events/event.h"
#include <chrono>

#include "AI/aitool.h"
#include "utilities/shortcut.h"

int main()
{
	Time timer;
	Window window;
	window.SetCursorPosCallBack(InputManager::MousePositionCallback);
	window.SetScrollCallBack(InputManager::MouseScrollCallback);

	window.Create(1920, 1080, "EnigmaEngine");
	window.SetWindowIcon("internal_assets/logos/LogoDark.png");
	InputManager::SetCurrentWindow(&window);

	Physics& physics = Physics::GetInstance();
	physics.Create();

	SceneManager::GetInstance().CreateScenes("config\\scenes.ini");
	Scene* scene = SceneManager::GetInstance().GetCurrentScene();

	Engine engine;

#ifndef BUILD_MODE
	Engine::SetBuildMode(EBuildMode::EDITOR);
#else
	Engine::SetBuildMode(EBuildMode::GAME);
#endif

	engine.CreateContext();

	float timeBeforeDeserialization = Time::GetTime();
	Serializer::GetInstance().DeserializeScene(scene);
	Debug::Log("Time to deserialize scene: " + ToString(Time::GetTime() - timeBeforeDeserialization) + " seconds");
	scene->LoadOnlyCurrentSceneAssets(scene, Engine::GetRHI());
	scene->RecreateModelsLoaded(Engine::GetRHI());
	Shortcut::Clear();

	ScriptManager& scriptManager = ScriptManager::GetInstance();
	SoundManager& soundManager = SoundManager::GetInstance();

	engine.CreateRenderer(&window);
	Engine::LinkDll();

#ifndef BUILD_MODE
	UI::UIEditor ui;
	ui.Create(&window, engine);
#endif

	if (Engine::IsInGameBuild())
	{
		Engine::LinkDll();
		Engine::LaunchGame();
	}

	Network::GetInstance().Create("config\\network.ini");
	Shortcut::Clear();

	while (!window.ShouldClose())
	{
		Scene* currentScene = SceneManager::GetInstance().GetCurrentScene();

		Network::GetInstance().Update();

#ifndef BUILD_MODE
		Engine::HotReload();
#endif
		timer.Update();
		soundManager.Update();

		if (Engine::IsRunning() && !Engine::IsPaused())
		{
			scriptManager.Update(timer.GetDeltaTime());

			while (timer.Accumulate())
			{
				scriptManager.FixedUpdate(timer.GetFixedDeltaTime());
				physics.FixedUpdate(timer.GetFixedDeltaTime());
			}

			physics.Update(timer.GetDeltaTime());
			scriptManager.LateUpdate(timer.GetDeltaTime());
		}

		if (std::array<int, 2> size = window.GetFrameBufferSize(); size[0] == 0 || size[1] == 0)
		{
			window.PollEvents();
			continue;
		}

		engine.UpdateRenderer(currentScene);
#ifndef BUILD_MODE
		ui.Update();
#endif
		window.SwapBuffers();
		window.PollEvents();
		InputManager::Update();
		engine.StopGame(timer);
	}

	Network::GetInstance().Destroy();
	SceneManager::GetInstance().SaveScenes();
	SceneManager::GetInstance().Destroy();
#ifndef BUILD_MODE
	ui.Destroy();
#endif
	engine.Destroy();
	physics.Destroy();

	return 0;
}
