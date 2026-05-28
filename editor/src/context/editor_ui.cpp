#include "context/editor_ui.h"

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include "panel/content_browser.h"
#include "panel/debug_console.h"
#include "panel/game_viewport.h"
#include "panel/inspector.h"
#include "panel/scene_hierarchy.h"
#include "panel/material_editor.h"
#include "scenes/scene_manager.h"
#include "context/style_manager.h"
#include "serialization/serializer.h"
#include "renderer/widget_renderer.h"

#include <iostream>

GameObject*  UI::UIEditor::selectedGameObject = nullptr;
unsigned int UI::UIEditor::gameTexture = 0;
unsigned int UI::UIEditor::viewportTexture = 0;
bool UI::UIEditor::makeGameSelected = false;
bool UI::UIEditor::requestMaterialEditor = false;
Material* UI::UIEditor::materialToDisplay = nullptr;
Engine UI::UIEditor::engine;

void UI::UIEditor::Create(Window* window, Engine& engine)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	UI::Style::SetConfigFile("Config/editor_config.ini");
	UI::Style::ApplyGeneralStyle();
	UI::Style::SetEnigmaFont();

	this->engine = engine;

	InitForGraphicsAPI(window);
	
	IUIPanel* contentBrowser = new ContentBrowserPanel();
	panels.push_back(contentBrowser);

	IUIPanel* debugConsole = new DebugConsolePanel();
	panels.push_back(debugConsole);

	IUIPanel* gameViewport = new GameViewportPanel();
	panels.push_back(gameViewport);

	IUIPanel* inspector = new InspectorPanel();
	panels.push_back(inspector);

	IUIPanel* sceneHierarchy = new SceneHierarchyPanel();
	panels.push_back(sceneHierarchy);

	IUIPanel* viewport = new ViewportPanel();
	panels.push_back(viewport);

	IUIPanel* materialEditor = new MaterialEditor();
	panels.push_back(materialEditor);
}

void UI::UIEditor::Update()
{
	NewFrame();

	SetMainScreenSpace(UpdateMenuBar());

	viewportTexture = engine.GetViewportTexture();
	gameTexture = engine.GetGameTexture();
	selectedGameObject = engine.GetRenderer()->GetSelectedGameObject();

	GetEditorInput();

	if (requestMaterialEditor)
	{
		for (IUIPanel* panel : panels)
		{
			panel->Draw();
		}
	}
	else
	{
		for (int i = 0; i < panels.size() - 1; i++)
		{
			panels[i]->Draw();
		}
	}

	EndFrame();
}

void UI::UIEditor::Destroy()
{
	for (int i = 0; i < panels.size(); i++)
		delete panels[i];

	DestroyAPIContext();
	ImGui::DestroyContext();
}

void UI::UIEditor::InitForGraphicsAPI(Window* window)
{
	switch (engine.GetGraphicsAPI())
	{
	case EnigmaRHI::ERenderAPI::OGL:
		ImGui_ImplGlfw_InitForOpenGL(window->GetWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 460");
		break;

	default:
		throw std::runtime_error("Unsupported API");
		break;
	}
}

void UI::UIEditor::NewFrame()
{
	switch (engine.GetGraphicsAPI())
	{
		ImGui::Render();
	case EnigmaRHI::ERenderAPI::OGL:
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
		break;
	default:
		throw std::runtime_error("Unsupported API");
		break;
	}
}

void UI::UIEditor::EndFrame()
{
	switch (engine.GetGraphicsAPI())
	{
	case EnigmaRHI::ERenderAPI::OGL:
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		break;
	default:
		throw std::runtime_error("Unsupported API");
		break;
	}
}

void UI::UIEditor::DestroyAPIContext()
{
	switch (engine.GetGraphicsAPI())
	{
	case EnigmaRHI::ERenderAPI::OGL:
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		break;
	default:
		throw std::runtime_error("Unsupported API");
		break;
	}
}

void UI::UIEditor::SetMainScreenSpace(float mainBarHeight)
{
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;

	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + mainBarHeight));
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - mainBarHeight));
	ImGui::SetNextWindowViewport(viewport->ID);

	windowFlags |= ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::Begin("Root", nullptr, windowFlags);

	ImGui::DockSpace(ImGui::GetID("Root"), ImVec2(0.0f, 0.0f), dockspace_flags);
	ImGui::End();
}

float UI::UIEditor::UpdateMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene", "Ctrl + N"))
			{
				Scene* newScene = SceneManager::GetInstance().AddScene();
				SceneManager::GetInstance().SetCurrentScene(newScene);
				Serializer::GetInstance().SerializeScene(newScene);
				GameObject* newGO = newScene->AddGameObject("Camera");
				newGO->AddComponent<GameCamera>();
			}
			if (ImGui::MenuItem("Save Scene", "Ctrl + S"))
			{
				engine.Save();
			}

			ImGui::EndMenu();
		}

		if (ImGui::Button("Hot-Reload", ImVec2{ 120, 25 }))
		{
			engine.RequestHotReload();
		}

		ImGui::EndMainMenuBar();
	}

	return ImGui::GetFrameHeight();
}

void UI::UIEditor::GetEditorInput()
{
	if (InputManager::GetKeyDown(KEY_LEFT_CONTROL))
	{
		if (InputManager::GetKeyClicked(KEY_S))
		{
			engine.Save();
		}
		else if(InputManager::GetKeyClicked(KEY_N))
		{
			LightManager::GetInstance().ClearLights();
			WidgetRenderer::GetInstance().ClearWidgets();
			Scene* newScene = SceneManager::GetInstance().AddScene();
			SceneManager::GetInstance().SetCurrentScene(newScene);
			GameObject* newGO = newScene->AddGameObject("Camera");
			newGO->AddComponent<GameCamera>();
		}
	}

	if (InputManager::GetKeyClicked(KEY_DELETE))
	{
		SceneManager::GetInstance().GetCurrentScene()->DeleteGameObject(selectedGameObject);
		engine.GetRenderer()->SetSelectedGameObject(nullptr);
	}

	if (InputManager::IsCursorHidden())
	{
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
		ImGui::GetIO().MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
	}
	else
		ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
}

