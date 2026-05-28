#include "panel/viewport.h"
#include <src/ImGuizmo.h>
#include "scenes/scene_manager.h"
#include "window/input_manager.h"
#include "context/editor_ui.h"
#include "components/mesh_renderer.h"
#include "resources/resource_manager.h"
#include "serialization/serializer.h"
#include "multithreading/async.h"

UI::ViewportPanel::ViewportPanel()
{
}

void UI::ViewportPanel::Draw()
{
	ImGui::SetNextWindowSizeConstraints(ImVec2(1, 1), ImVec2(FLT_MAX, FLT_MAX));

	if(UIEditor::engine.IsRunning())
		UI::Style::PushInGameStyle();
	else
		UI::Style::PushMenuBarStyle();

	ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_MenuBar);
	ImGui::BeginMenuBar();

	AddMenuBarButton();

	UI::Style::PopMenuBarStyle();

	ImGui::EndMenuBar();

	ImVec2 windowPos = ImGui::GetCursorScreenPos();
	float width = ImGui::GetContentRegionAvail().x;
	float height = ImGui::GetContentRegionAvail().y;

	UIEditor::engine.SetViewportSize(width * 2.f, height * 2.f);

	ImGui::Image
	(
		(ImTextureID)UIEditor::viewportTexture,
		ImGui::GetContentRegionAvail(),
		ImVec2(0, 1),
		ImVec2(1, 0)
	);

	if (ImGui::IsItemHovered())
	{
		InputManager::UnblockInput();

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			canPickObject = true;

		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && !ImGuizmo::IsUsing() && canPickObject)
		{
			UIEditor::engine.PickObject({ width, height }, { windowPos.x, windowPos.y });
			canPickObject = false;
		}
	}
	else
	{
		if (!SceneManager::GetInstance().GetCurrentScene()->GetEngineCam()->IsUsing())
			InputManager::BlockInput();
	}

	GetGuizmoInputs();

	Scene* scene = SceneManager::GetInstance().GetCurrentScene();

	ComputeDragAndDrop(scene);
	ComputeGizmos(scene, { windowPos.x, windowPos.y }, { width, height });
}

void UI::ViewportPanel::GetGuizmoInputs()
{
	if (SceneManager::GetInstance().GetCurrentScene()->GetEngineCam()->IsUsing())
		return;

	if (InputManager::GetKeyDown(KEY_W))
		currentGizmoMode = EGuizmoMode::TRANSLATE;

	else if (InputManager::GetKeyDown(KEY_R))
		currentGizmoMode = EGuizmoMode::ROTATE;

	else if (InputManager::GetKeyDown(KEY_E))
		currentGizmoMode = EGuizmoMode::SCALE;
}

void UI::ViewportPanel::AddMenuBarButton()
{
	float padding = 2.5f;
	ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x / 2.f) - ImGui::GetFontSize() - (2.f * padding));

	unsigned int playPauseTexture = UIEditor::engine.GetEditorTexture(UIEditor::engine.IsRunning() ? "pause_icon.png" : "play_icon.png");

	if (ImGui::ImageButton("##Play", (ImTextureID)playPauseTexture, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() }))
	{
		if (UIEditor::engine.IsRunning())
			UIEditor::engine.PauseGame();
		else
			UIEditor::engine.LaunchGame();

		UIEditor::makeGameSelected = true;
	}

	if (ImGui::ImageButton("##Stop", (ImTextureID)UIEditor::engine.GetEditorTexture("stop_icon.png"), ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() }))
		UIEditor::engine.RequestStopGame();

	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - (3.f * ImGui::GetFontSize()) - (2.f * padding));

	bool displayAABB = UIEditor::engine.DisplayAABB();
	UI::CreateCheckboxImage("DisplayAABB", &displayAABB, UIEditor::engine.GetEditorTexture("AABB_icon.png"), UIEditor::engine.GetEditorTexture("non_AABB_icon.png"));
	UIEditor::engine.SetAABBDisplayActive(displayAABB);

	bool showGizmos = UIEditor::engine.IsGizmosActive();
	UI::CreateCheckboxImage("ShowGizmos", &showGizmos, UIEditor::engine.GetEditorTexture("gizmo_renderer_icon.png"), UIEditor::engine.GetEditorTexture("non_gizmo_renderer_icon.png"));
	UIEditor::engine.SetGizmosActive(showGizmos);
}

void UI::ViewportPanel::ComputeDragAndDrop(Scene* scene)
{
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags dropTargetFlags = ImGuiDragDropFlags_AcceptBeforeDelivery;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(UI::ToLabel(EDragAndDropType::FILE3D), dropTargetFlags))
		{
			if (InputManager::GetMouseButtonUp(Key::MOUSE_BUTTON_LEFT))
			{
				std::string fileName = std::string(static_cast<const char*>(payload->Data), payload->DataSize);
				std::filesystem::path p = fileName;
				GameObject* newGO = scene->AddGameObject(p.stem().string());

				if (Model* model = ResourceManager::GetInstance().Get<Model>(fileName))
				{
					Model::InstantiateIntoScene(model, newGO, scene);
				}
				else
				{
					auto CreateNewGameObject = [fileName]()
						{
							ResourceManager::GetInstance().Create<Model>(fileName, UIEditor::engine.GetRHI());
						};
					Async::GetInstance().CreateTask(CreateNewGameObject);
					UIEditor::engine.AddModelToAwait(fileName, newGO, scene);
				}

				newGO->transform.position = scene->GetEngineCam()->GetPosition() + scene->GetEngineCam()->GetForward() * 2.f;
				UIEditor::engine.GetRenderer()->SetSelectedGameObject(newGO);
			}
		}
		else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(UI::ToLabel(EDragAndDropType::TEXTURE_FILE), dropTargetFlags))
		{
			if (InputManager::GetMouseButtonUp(Key::MOUSE_BUTTON_LEFT))
			{
				std::string fileName = std::string(static_cast<const char*>(payload->Data), payload->DataSize);
				scene->SetSkyboxPath("assets\\skyboxes\\" + fileName);
			}
		}
		else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(UI::ToLabel(EDragAndDropType::PREFAB_FILE), dropTargetFlags))
		{
			if (InputManager::GetMouseButtonUp(Key::MOUSE_BUTTON_LEFT))
			{
				std::string fileName = std::string(static_cast<const char*>(payload->Data), payload->DataSize);
				std::filesystem::path path = fileName;
				Serializer::GetInstance().ReadPrefab(scene, path.stem().string());
				GameObject* newGO = scene->GetGameObject(scene->GetGameObjects().size() - 1);

				if (MeshRenderer* mr = newGO->GetComponent<MeshRenderer>())
				{
					Model* model = ResourceManager::GetInstance().Get<Model>(mr->modelName);
					if (!model)
					{
						model = ResourceManager::GetInstance().Create<Model>(newGO->GetComponent<MeshRenderer>()->modelName, UIEditor::engine.GetRHI());
						model->CreateGPUResources(UIEditor::engine.GetRHI());
					}
				}
				newGO->transform.position = scene->GetEngineCam()->GetPosition() + scene->GetEngineCam()->GetForward() * 2.f;
				UIEditor::engine.GetRenderer()->SetSelectedGameObject(newGO);
			}
		}
		else
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
			ImGui::SetTooltip("Cannot drop here!");
		}

		ImGui::EndDragDropTarget();
	}
}

void UI::ViewportPanel::ComputeGizmos(Scene* scene, Math::Vector2D windowPos, Math::Vector2D windowSize) const
{
	GameObject* selectedGameObject = UIEditor::engine.GetRenderer()->GetSelectedGameObject();

	if (!selectedGameObject)
	{
		ImGui::End();
		return;
	}

	if (selectedGameObject->IsUIElement())
	{
		ImGui::End();
		return;
	}

	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);

	float matrix[16];
	memcpy(matrix, selectedGameObject->transform.global.m, sizeof(float) * 16);

	ImGuizmo::Manipulate(scene->GetEngineCam()->GetView().m, scene->GetEngineCam()->GetProjection().m, (ImGuizmo::OPERATION)currentGizmoMode, ImGuizmo::LOCAL, matrix);

	if (ImGuizmo::IsUsing())
	{
		if (InputManager::GetMouseButtonDown(Key::MOUSE_BUTTON_RIGHT))
		{
			ImGui::End();
			return;
		}

		Math::Matrix4x4 modifiedGlobalMatrix;
		memcpy(modifiedGlobalMatrix.m, matrix, sizeof(float) * 16);

		Math::Vector3D localPos, localRot, localScale;

		if (selectedGameObject->GetParent())
		{
			Math::Matrix4x4 parentGlobalInverse = selectedGameObject->GetParent()->transform.global.Inverse();
			Math::Matrix4x4 localMatrix = parentGlobalInverse * modifiedGlobalMatrix;
			ImGuizmo::DecomposeMatrixToComponents(localMatrix.m, &localPos.x, &localRot.x, &localScale.x);
		}
		else
		{
			ImGuizmo::DecomposeMatrixToComponents(modifiedGlobalMatrix.m, &localPos.x, &localRot.x, &localScale.x);
		}

		bool changedScale = (localScale != selectedGameObject->transform.scale);

		selectedGameObject->SetTransform(localPos, localRot, localScale);

		if (Physicalbody* pb = selectedGameObject->physicalBody)
			selectedGameObject->physicalBody->UpdatePhysicsTransform(changedScale);

		for (GameObject* child : selectedGameObject->GetChildren())
		{
			if (Physicalbody* pb = selectedGameObject->physicalBody)
				pb->UpdatePhysicsTransform(changedScale);
		}
	}

	ImGui::End();
}
