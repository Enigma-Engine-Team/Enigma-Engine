#include "network/network.h"
#include "context/editor_ui.h"
#include "scenes/scene_manager.h"
#include "timer/timer.h"
#include "engine/engine.h"
#include "scripting/script_manager.h"
#include "physics/physics.h"
#include "serialization/serializer.h"
#include "sound/sound_manager.h"
#include "events/event.h"
#include <chrono>

int main()
{
	Window window;
	window.SetCursorPosCallBack(InputManager::MousePositionCallback);
	window.SetScrollCallBack(InputManager::MouseScrollCallback);

	window.Create(1920, 1080, "EnigmaEngine");
	window.SetWindowIcon("internal_assets/logos/LogoDark.png");

	Physics& physics = Physics::GetInstance();
	physics.Create();

	SceneManager::GetInstance().CreateScenes("config\\scenes.ini");
	Scene* scene = SceneManager::GetInstance().GetCurrentScene();

	Engine engine;

#ifndef BUILD_MODE
	engine.SetBuildMode(EBuildMode::EDITOR);
#else
	engine.SetBuildMode(EBuildMode::GAME);
#endif

	engine.CreateContext();

	Serializer::GetInstance().DeserializeScene(scene);
	scene->LoadOnlyCurrentSceneAssets(scene, engine.GetRHI());
	scene->RecreateModelsLoaded(engine.GetRHI());

	ScriptManager& scriptManager = ScriptManager::GetInstance();
	SoundManager& soundManager = SoundManager::GetInstance();
	Timer timer;

	engine.CreateRenderer(&window);

#ifndef BUILD_MODE
	UI::UIEditor ui;
	ui.Create(&window, engine);
#endif

	if (engine.IsInGameBuild())
	{
		engine.LinkDll();
		engine.LaunchGame();
	}

	Network::GetInstance().Create("config\\network.ini");

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