#include "context/ui_utils.h"
#include <string>
#include "scenes/scene_manager.h"
#include "context/editor_ui.h"
#include "components/mesh_renderer.h"
#include "components/material_manager.h"

bool UI::CreateVector3Control(const char* label, Math::Vector3D* vec, bool activeLinkButton)
{
	bool returnValue = false;
	ImGui::PushID(label);
	ImGui::Text(label);

	const float dragSpeed = 0.1f;

	const float normalWindowWidth = 485.0f;
	const float minWindowSize = 200.f;

	float normalSliderWidth = 80.0f;
	const float minSliderWidth = 30.0f;
	float normalSpaceTextSlider = 10.0f;
	const float minSpaceTextSlider = 4.0f;

	const char* axisLabels[3] = { "X", "Y", "Z" };
	float* components[3] = { &vec->x, &vec->y, &vec->z };

	static bool linkAxes = false;

	if (activeLinkButton)
	{
		ImGui::SameLine(0.f, 20.f);

		CreateCheckboxImage("LinkAxes", &linkAxes, UIEditor::engine.GetEditorTexture("link_icon.png"), UIEditor::engine.GetEditorTexture("non_link_icon.png"));
	}

	ImGui::SameLine();

	for (int i = 0; i < 3; ++i)
	{
		if (i > 0)
			ImGui::SameLine(0.0f, normalSpaceTextSlider);
		else if (i == 0)
		{
			ImGui::SetCursorPosX(150.f);
		}

		ImGui::PushID(i);

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(axisLabels[i]);

		ImGui::SameLine(0.0f, normalSpaceTextSlider);

		SetVector3ControlWidth(normalSliderWidth, minWindowSize, minSliderWidth, normalWindowWidth);

		if (activeLinkButton)
		{
			if (ImGui::DragFloat("##value", components[i], dragSpeed, 0.0f, 10000.f, "%.2f"))
				returnValue = true;
		}
		else
		{
			if(ImGui::DragFloat("##value", components[i], dragSpeed, 0.0f, 0.0f, "%.2f"))
				returnValue = true;
		}


		if (activeLinkButton && linkAxes)
		{
			for (int j = 0; j < 3; ++j)
			{
				if (j != i)
					*components[j] = *components[i];
			}
		}

		ImGui::PopID();
	}

	ImGui::PopID();

	return returnValue;
}

bool UI::CreateVector2Control(const char* label, Math::Vector2D* vec)
{
	bool returnValue = false;
	ImGui::PushID(label);
	ImGui::Text(label);

	const float dragSpeed = 0.1f;

	const float normalWindowWidth = 485.0f;
	const float minWindowSize = 200.f;

	float normalSliderWidth = 80.0f;
	const float minSliderWidth = 30.0f;
	float normalSpaceTextSlider = 10.0f;
	const float minSpaceTextSlider = 4.0f;

	const char* axisLabels[2] = { "X", "Y" };
	float* components[2] = { &vec->x, &vec->y };

	ImGui::SameLine();

	for (int i = 0; i < 2; ++i)
	{
		if (i > 0)
			ImGui::SameLine(0.0f, normalSpaceTextSlider);
		else if (i == 0)
		{
			ImGui::SetCursorPosX(150.f);
		}

		ImGui::PushID(i);

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(axisLabels[i]);

		ImGui::SameLine(0.0f, normalSpaceTextSlider);

		SetVector3ControlWidth(normalSliderWidth, minWindowSize, minSliderWidth, normalWindowWidth);

		if (ImGui::DragFloat("##value", components[i], dragSpeed, 0.0f, 0.0f, "%.2f"))
			returnValue = true;


		ImGui::PopID();
	}

	ImGui::PopID();

	return returnValue;
}


void UI::SetVector3ControlWidth(float itemWidth, float minWindowWidth, float minItemWidth, float windowWidth)
{
	if (ImGui::GetWindowSize().x <= minWindowWidth)
	{
		ImGui::SetNextItemWidth(minItemWidth);
	}
	else if (ImGui::GetWindowSize().x < windowWidth)
	{
		float newSliderWidth = itemWidth - ((windowWidth - ImGui::GetWindowSize().x) / 3.f);
		ImGui::SetNextItemWidth(newSliderWidth);
	}
	else
		ImGui::SetNextItemWidth(itemWidth);
}

bool UI::TreeNodeImage(const char* text, unsigned int textureID, int flags)
{
	bool open = ImGui::TreeNodeEx(text, flags, "");

	if (ImGui::BeginDragDropSource())
	{
		Scene* currentScene = SceneManager::GetInstance().GetCurrentScene();
		ImGui::SetDragDropPayload(UI::ToLabel(EDragAndDropType::SCENE_FILE), &currentScene, sizeof(Scene*));
		ImGui::Text(currentScene->GetName().c_str());
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags dropTargetFlags = ImGuiDragDropFlags_AcceptBeforeDelivery;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(UI::ToLabel(EDragAndDropType::GAMEOBJECT), dropTargetFlags))
		{
			if (InputManager::GetMouseButtonUp(Key::MOUSE_BUTTON_LEFT))
			{
				GameObject* target = *static_cast<GameObject**>(payload->Data);
				target->SetParent(SceneManager::GetInstance().GetCurrentScene()->GetGameObject(0));
			}
		}
		else
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
			ImGui::SetTooltip("Cannot drop here!");
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();
	int iconSize = ImGui::GetFontSize();
	float iconPadding = 3.0f;
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + iconPadding);
	ImGui::Image((ImTextureID)textureID, ImVec2(iconSize, iconSize));

	ImGui::SameLine();
	ImGui::Text(text);

	return open;
}

bool UI::TreeNodeImage(GameObject* obj, unsigned int textureID, int flags)
{
	bool open = ImGui::TreeNodeEx((void*)obj, flags, "");

	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload(UI::ToLabel(EDragAndDropType::GAMEOBJECT), &obj, sizeof(GameObject*));
		ImGui::Text(obj->GetName().c_str());
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags dropTargetFlags = ImGuiDragDropFlags_AcceptBeforeDelivery;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(UI::ToLabel(EDragAndDropType::GAMEOBJECT), dropTargetFlags))
		{
			if (InputManager::GetMouseButtonUp(Key::MOUSE_BUTTON_LEFT))
			{
				GameObject* target = *static_cast<GameObject**>(payload->Data);
				obj->AddChild(target);
			}
		}
		else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(UI::ToLabel(EDragAndDropType::MATERIAL_FILE), dropTargetFlags))
		{
			if (InputManager::GetMouseButtonUp(Key::MOUSE_BUTTON_LEFT))
			{
				std::string fileName(static_cast<const char*>(payload->Data), payload->DataSize);
				std::filesystem::path path = fileName;
				fileName = path.stem().string();
				MeshRenderer* meshRenderer = obj->GetComponent<MeshRenderer>();
				if (meshRenderer)
				{
					meshRenderer->SetMaterial(MaterialManager::GetInstance().GetMaterial(fileName));
					meshRenderer->GetMaterial()->CreateMaterialData(UIEditor::engine.GetRHI());
				}
			}
		}
		else
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
			ImGui::SetTooltip("Cannot drop here!");
		}

		ImGui::EndDragDropTarget();
	}

	static bool canSelectGameObject = false;

	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		canSelectGameObject = true;

	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImGui::IsItemHovered() && !ImGui::IsItemToggledOpen() && canSelectGameObject)
	{
		UIEditor::engine.GetRenderer()->SetSelectedGameObject(obj);
		canSelectGameObject = false;
	}

	ImGui::SameLine();
	int iconSize = ImGui::GetFontSize();
	float iconPadding = 3.0f;
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + iconPadding);
	ImGui::Image((ImTextureID)textureID, ImVec2(iconSize, iconSize));

	ImGui::SameLine();
	ImGui::Text(obj->GetName().c_str());

	return open;
}

bool UI::CollapsingHeaderImage(const char* name, unsigned int textureID, int flags)
{
	int iconSize = ImGui::GetFontSize();
	float iconPadding = 3.0f;
	ImGui::SetNextItemAllowOverlap();
	bool isOpen = ImGui::CollapsingHeader(("##" + std::to_string(textureID)).c_str(), flags);
	ImGui::SameLine();
	ImGui::SameLine();
	ImGui::SetCursorPos({ ImGui::GetCursorPosX() + iconPadding, ImGui::GetCursorPosY() + iconPadding });
	ImGui::Image((ImTextureID)textureID, ImVec2(iconSize, iconSize));
	ImGui::SameLine();
	ImGui::Text(name);

	return isOpen;
}

void UI::CreateCheckboxImage(const char* name, bool* boolean, unsigned int textureTrue, unsigned int textureFalse)
{
	float iconSize = ImGui::GetFontSize();
	unsigned int linkTexture = *boolean ? textureTrue : textureFalse;

	std::string label = std::string("##") + name;

	if (ImGui::ImageButton(label.c_str(), (ImTextureID)(uintptr_t)linkTexture, ImVec2{ iconSize, iconSize }))
	{
		*boolean = !*boolean;
	}
}

void UI::ImageText(const char* text, unsigned int textureID)
{
	float iconSize = ImGui::GetFontSize();
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.5f);
	ImGui::Image(textureID, ImVec2{ iconSize, iconSize });
	ImGui::SameLine();
	ImGui::Text(text);
}

void UI::ImageTextButton(const char* text, unsigned int textureID, Math::Vector2D size)
{
	ImVec2 buttonCursor = ImGui::GetCursorPos();
	std::string label = text;
	ImGui::Button(("##value" + label).c_str(), ImVec2{ size.x, size.y });
	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() - (size.x * 0.5f) - ImGui::CalcTextSize(text).x * 0.5f - ImGui::GetFontSize());
	ImageText(text, textureID);
	ImGui::SetCursorPos(buttonCursor);
	ImGui::InvisibleButton(("##value2" + label).c_str(), ImVec2{size.x, size.y});
}

void UI::DrawNodeBackground(ImVec4 color)
{
	ImVec2 pos = ImGui::GetCursorScreenPos();
	float width = ImGui::GetContentRegionAvail().x;
	float height = ImGui::GetFrameHeight();

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImU32 col = ImGui::ColorConvertFloat4ToU32(color);
	draw_list->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height - 5.f), col);
}

bool UI::RecursiveTreeNode(GameObject* currentGameObject, unsigned int textureID, int flags)
{
	bool noChildren = currentGameObject->GetChildren().empty();
	if (noChildren)
		flags |= ImGuiTreeNodeFlags_Leaf;

	bool open = false;

	if (currentGameObject == UIEditor::selectedGameObject)
	{
		DrawNodeBackground(Color::EnigmaBaseColorFull);

		UI::Style::PushStyle(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
		UI::Style::PushStyle(ImGuiCol_HeaderHovered, UI::Color::EnigmaBaseColorLighter);
		open = UI::TreeNodeImage(currentGameObject, textureID, flags);
		UI::Style::PopStyle();
	}
	else
	{
		open = UI::TreeNodeImage(currentGameObject, textureID, flags);
	}

	if (open)
	{
		if (!noChildren)
		{
			for (int i = 0; i < currentGameObject->GetChildren().size(); i++)
			{
				if (RecursiveTreeNode(currentGameObject->GetChildren()[i], textureID, flags))
				{
					ImGui::TreePop();
				}
			}
		}
		return open;
	}
	return open;
}

bool UI::RecursiveTreeNode(std::string path, unsigned int textureID, int flags, std::filesystem::path& currentDirectory)
{
	std::vector<std::string> subDirectories;
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        if (entry.is_directory())
            subDirectories.push_back(entry.path().string());
    }

    bool noChildren = subDirectories.empty();
    if (noChildren)
        flags |= ImGuiTreeNodeFlags_Leaf;


    std::string folderName = std::filesystem::path(path).filename().string();
    bool open = ImGui::TreeNodeEx(folderName.c_str(), flags, "");

	if (ImGui::IsItemClicked())
		currentDirectory = path;

	ImGui::SameLine();
	int iconSize = ImGui::GetFontSize();
	float iconPadding = 3.0f;
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + iconPadding);
	ImGui::Image((ImTextureID)textureID, ImVec2(iconSize, iconSize));

	ImGui::SameLine();
	ImGui::Text(folderName.c_str());

    if (!noChildren)
    {
        if (open)
        {
            for (const auto& subDir : subDirectories)
                RecursiveTreeNode(subDir, textureID, flags, currentDirectory);

            ImGui::TreePop();
        }
        return open;
		
    }

	if(open)
		ImGui::TreePop();
    return open;
}

const char* UI::ToLabel(EDragAndDropType type)
{
	switch (type)
	{
	case EDragAndDropType::UNDEFINED:
		return "UNDEFINED";
	case EDragAndDropType::GAMEOBJECT:
		return "GAMEOBJECT";
	case EDragAndDropType::TEXTURE_FILE:
		return "TEXTURE";
	case EDragAndDropType::MATERIAL_FILE:
		return "MATERIAL";
	case EDragAndDropType::SCENE_FILE:
		return "SCENE";
	case EDragAndDropType::FILE3D:
		return "FILE3D";
	case EDragAndDropType::SOUND_FILE:
		return "SOUND_FILE";
	case EDragAndDropType::FONT_FILE:
		return "FONT_FILE";
	case EDragAndDropType::PREFAB_FILE:
		return "PREFAB_FILE";
	}
}