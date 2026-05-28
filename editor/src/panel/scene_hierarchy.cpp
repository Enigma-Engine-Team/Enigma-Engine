#include "panel/scene_hierarchy.h"
#include "scenes/scene_manager.h"
#include "context/editor_ui.h"
#include "components/mesh_renderer.h"
#include "components/text.h"
#include "components/button.h"
#include "resources/resource_manager.h"
#include "serialization/serializer.h"
#include "components/material_manager.h"

UI::SceneHierarchyPanel::SceneHierarchyPanel()
{
	objectID = UIEditor::engine.GetEditorTexture("object_icon.png");
	sceneID = UIEditor::engine.GetEditorTexture("scene_icon.png");
}

void UI::SceneHierarchyPanel::Draw()
{
	ImGui::Begin("SceneHierarchy", nullptr, ImGuiWindowFlags_MenuBar);

	Scene* currentScene = SceneManager::GetInstance().GetCurrentScene();

	bool shouldOpenMaterialModal = false;

	if (ImGui::BeginMenuBar())
	{
		UI::Style::PushMenuBarStyle();
		if (ImGui::ImageButton("AddButton", (ImTextureID)UIEditor::engine.GetEditorTexture("add_icon.png"), ImVec2(31, 22)))
			ImGui::OpenPopup("AddObjectPopup");
		UI::Style::PopMenuBarStyle();

		if (ImGui::BeginPopup("AddObjectPopup"))
		{
			GameObject* newGO = nullptr;
			Math::Vector3D newPos = currentScene->GetEngineCam()->GetPosition() + currentScene->GetEngineCam()->GetForward() * 2.f;
			Math::Quaternion newRot = currentScene->GetEngineCam()->GetDirection();

			if (ImGui::MenuItem("Create Empty"))
			{
				newGO = currentScene->AddGameObject("Empty Object");
			}

			if (ImGui::MenuItem("Camera"))
			{
				newGO = currentScene->AddGameObject("Camera");
				newGO->AddComponent<GameCamera>();
			}

			if (ImGui::BeginMenu("3D Object"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					newGO = currentScene->AddGameObject("Cube");
					Model::InstantiateIntoScene(ResourceManager::GetInstance().Get<Model>("cube.gltf"), newGO, currentScene);
				}
				if (ImGui::MenuItem("Sphere"))
				{
					newGO = currentScene->AddGameObject("Sphere");

					if (Model* model = ResourceManager::GetInstance().Get<Model>("sphere.gltf"))
					{
						Model::InstantiateIntoScene(model, newGO, currentScene);
					}
					else
					{
						Model* mesh = ResourceManager::GetInstance().Create<Model>("sphere.gltf", UIEditor::engine.GetRHI());
						mesh->CreateGPUResources(UIEditor::engine.GetRHI());
						Model::InstantiateIntoScene(mesh, newGO, currentScene);
					}
				}

				ImGui::EndMenu();
			}


			if (ImGui::BeginMenu("Light"))
			{
				if (ImGui::MenuItem("Directionnal Light"))
				{
					newGO = currentScene->AddGameObject("Directional Light");
					Light* lightComp = newGO->AddComponent<Light>();
					lightComp->SetLightTypeEnum(ELightType::DIRECTIONAL);
					newGO->transform.rotation = newRot;
				}
				if (ImGui::MenuItem("Spot Light"))
				{
					newGO = currentScene->AddGameObject("Spot Light");
					Light* lightComp = newGO->AddComponent<Light>();
					lightComp->SetLightTypeEnum(ELightType::SPOT);
					newGO->transform.rotation = newRot;
				}

				if (ImGui::MenuItem("Point Light"))
				{
					newGO = currentScene->AddGameObject("Point Light");
					Light* lightComp = newGO->AddComponent<Light>();
					lightComp->SetLightTypeEnum(ELightType::POINT);
				}

				ImGui::EndMenu();
			}

			if (newGO)
				newGO->transform.position = newPos;

			if (ImGui::BeginMenu("GUI Items"))
			{
				if (ImGui::MenuItem("2D Text"))
				{
					newGO = currentScene->AddGameObject("Text");
					newGO->transform.position = { 0.f, 0.f, 0.f };
					newGO->AddComponent<Text>();
				}
				if (ImGui::MenuItem("Button"))
				{
					newGO = currentScene->AddGameObject("Button");
					newGO->transform.position = { 0.f, 0.f, 0.f };
					newGO->AddComponent<Button>();
				}

				ImGui::EndMenu();
			}

			if (newGO)
				UIEditor::engine.GetRenderer()->SetSelectedGameObject(newGO);

			if (ImGui::MenuItem("Create Material"))
			{
				shouldOpenMaterialModal = true;
			}

			ImGui::EndPopup();
		}

		ImGui::EndMenuBar();
	}

	
	if (shouldOpenMaterialModal)
	{
		ImGui::OpenPopup("New Material");
	}

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("New Material", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static char buffer[256] = "";
		ImGui::InputText("Name", buffer, sizeof(buffer));

		if (ImGui::Button("Create"))
		{ 
			Material* mat  = MaterialManager::GetInstance().AddMaterial(buffer);
			mat->CreateMaterialData(Engine::GetRHI());

			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel")) 
		{ 
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	RenderHierarchy(currentScene);
	ImGui::End();
}

void UI::SceneHierarchyPanel::RenderHierarchy(Scene* currentScene) const
{
	ImGuiTreeNodeFlags sceneFlag = ImGuiTreeNodeFlags_OpenOnArrow;
	sceneFlag |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
	sceneFlag |= ImGuiTreeNodeFlags_SpanAvailWidth;

	if (TreeNodeImage(currentScene->GetName().c_str(), sceneID, sceneFlag | ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (int i = 1; i < currentScene->GetGameObjects().size(); i++)
		{
			if (currentScene->GetGameObject(i)->GetParent() == currentScene->GetGameObject(0))
			{
				if (RecursiveTreeNode(currentScene->GetGameObject(i), objectID, sceneFlag))
				{
					ImGui::TreePop();
				}
			}
		}
		ImGui::TreePop();
	}
}
