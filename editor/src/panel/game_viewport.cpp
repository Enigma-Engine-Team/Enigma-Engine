#include "panel/game_viewport.h"
#include "context/editor_ui.h"
#include "scenes/scene_manager.h"

UI::GameViewportPanel::GameViewportPanel()
{
}

void UI::GameViewportPanel::Draw()
{
	if (UIEditor::makeGameSelected)
	{
		ImGui::SetNextWindowFocus();
		UIEditor::makeGameSelected = false;
	}

    ImGui::Begin("Game");

	float gameWidth = 1920.f;
	float gameHeight = 1080.f;

    ImVec2 available = ImGui::GetContentRegionAvail();

    float screenFormat = 16.f / 9.f;

    float width = available.x;
    float height = width / screenFormat;

    if (height > available.y)
    {
        height = available.y;
        width = height * screenFormat;
    }

    ImGui::SetCursorPosX((available.x - width) * 0.5f);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (available.y - height) * 0.5f);

	ImVec2 windowPos = ImGui::GetCursorScreenPos();

	ImGui::Image(
		(ImTextureID)UIEditor::engine.GetGameTexture(),
		ImVec2(width, height),
		ImVec2(0, 1),
		ImVec2(1, 0)
	);

	GameObject* selectedGameObject = UIEditor::engine.GetRenderer()->GetSelectedGameObject();

	if (selectedGameObject && selectedGameObject->IsUIElement())
	{
		ImGuizmo::SetOrthographic(true);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(windowPos.x, windowPos.y, width, height);
	}

	Scene* scene = SceneManager::GetInstance().GetCurrentScene();

	float borderX = available.x - width;
	float borderY = available.y - height;

	ImVec2 windowLocalPos = ImGui::GetWindowPos();
	ImVec2 mousePos = ImGui::GetMousePos();

	ImVec2 relative = ImVec2
	{
		mousePos.x - windowLocalPos.x - (borderX / 2.f),
		mousePos.y - windowLocalPos.y - (borderY / 2.f) - 36.f // - 36 for main menu bar
	};

	InputManager::UpdateGameRect({ relative.x, relative.y }, width, height);

	if (!selectedGameObject || !selectedGameObject->IsUIElement())
	{
		ImGui::End();
		return;
	}

	float matrix[16];
	memcpy(matrix, selectedGameObject->transform.global.m, sizeof(float) * 16);

	Math::Matrix4x4 projection = Math::Matrix4x4::Orthographic(-gameWidth * 0.5f, gameWidth * 0.5f, -gameHeight * 0.5f, gameHeight * 0.5f, -10.f, 10.f);
	Math::Matrix4x4 view = Math::Matrix4x4::LookAt(Math::Vector3D(0.f, 0.f, 1.f), Math::Vector3D(0.f, 0.f, -1.f), Math::Vector3D::Up);

	ImGuizmo::Manipulate(
		view.m,
		projection.m,
		ImGuizmo::OPERATION::TRANSLATE,
		ImGuizmo::WORLD,
		matrix
	);

	if (ImGuizmo::IsUsing())
	{
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

		if (!selectedGameObject->physicalBody)
		{
			ImGui::End();
			return;
		}
		selectedGameObject->physicalBody->UpdatePhysicsTransform(changedScale);

		//Only work for direct children, need to be updated for gran children
		for (GameObject* child : selectedGameObject->GetChildren())
		{
			child->physicalBody->UpdatePhysicsTransform(changedScale);
		}
	}

    ImGui::End();
}
