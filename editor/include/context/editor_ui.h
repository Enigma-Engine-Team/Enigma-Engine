#pragma once
#include "panel.h"
#include "window/window.h"
#include "renderer/renderer.h"
#include "engine/engine.h"

namespace UI
{
	class UIEditor
	{
	public:
		void Create(Window* window, Engine& engine);
		void Update();
		void Destroy();

		static Engine engine;
		static GameObject* selectedGameObject;
		static unsigned int gameTexture;
		static unsigned int viewportTexture;
		static bool makeGameSelected;
		static bool requestMaterialEditor;
		static Material* materialToDisplay;

	private:

		void InitForGraphicsAPI(Window* window);
		void NewFrame();
		void EndFrame();
		void DestroyAPIContext();
		void SetMainScreenSpace(float mainBarHeight);
		float UpdateMenuBar();
		void GetEditorInput();
		std::vector<IUIPanel*> panels;
	};
}