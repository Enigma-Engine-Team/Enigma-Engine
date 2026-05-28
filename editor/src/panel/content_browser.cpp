#include "panel/content_browser.h"
#include "IImage.h"
#include "resources/texture.h"
#include "resources/resource_manager.h"
#include "context/editor_ui.h"
#include "renderer/widget_renderer.h"
#include "scenes/scene_manager.h"
#include "components/button.h"
#include "components/text_mesh.h"
#include "components/text.h"
#include "components/material_manager.h"
#include "renderer/light_manager.h"

UI::ContentBrowserPanel::ContentBrowserPanel()
	:currentDirectory(assetPath)
{
	folderTextureID = UIEditor::engine.GetEditorTexture("folder_icon.png");
	commonFileTextureID = UIEditor::engine.GetEditorTexture("common_file_icon.png");
	meshFileTextureID = UIEditor::engine.GetEditorTexture("3D_file_icon.png");
	soundFileTextureID = UIEditor::engine.GetEditorTexture("sound_file_icon.png");
	textureFileTextureID = UIEditor::engine.GetEditorTexture("texture_file_icon.png");
	materialFileTextureID = UIEditor::engine.GetEditorTexture("material_file_icon.png");
	fontFileTextureID = UIEditor::engine.GetEditorTexture("font_file_icon.png");
	sceneFileTextureID = UIEditor::engine.GetEditorTexture("scene_file_icon.png");
	prefabFileTextureID = UIEditor::engine.GetEditorTexture("prefab_file_icon.png");
}

void UI::ContentBrowserPanel::Draw()
{
	ImGui::Begin("Content Browser");

	static float w = 200.0f;

	ImGui::BeginChild("treenode_folder", ImVec2(w, 0), true);

	ImGuiTreeNodeFlags folderFlag = ImGuiTreeNodeFlags_DefaultOpen;
	folderFlag |= ImGuiTreeNodeFlags_OpenOnArrow;
	folderFlag |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
	folderFlag |= ImGuiTreeNodeFlags_SpanAvailWidth;

	UI::RecursiveTreeNode(assetPath.string(), folderTextureID, folderFlag, currentDirectory);

	ImGui::EndChild();

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::SameLine();
	ImGui::InvisibleButton("vSplitter", ImVec2(8.0f, ImGui::GetContentRegionAvail().y));

	if (ImGui::IsItemHovered() || ImGui::IsItemActive())
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

	if (ImGui::IsItemActive())
	{
		w += ImGui::GetIO().MouseDelta.x;
		w = std::clamp(w, 20.f, ImGui::GetWindowSize().x - 40.f);
	}
	ImGui::SameLine();
	ImGui::PopStyleVar();

	ImGui::BeginChild("files", ImVec2(0, 0), 0, ImGuiWindowFlags_MenuBar);

	ImGui::BeginMenuBar();

	DrawFolderListPath();

	ImGui::EndMenuBar();

	DrawFileColumnsInCurrentFolder();
	
	ImGui::EndChild();

	ImGui::End();
}

void UI::ContentBrowserPanel::DrawFolderListPath()
{
	if (currentDirectory != assetPath)
	{
		std::filesystem::path relativePath = std::filesystem::relative(currentDirectory, assetPath);

		std::filesystem::path built = assetPath;

		ImVec2 assetTextSize = ImGui::CalcTextSize(assetPath.string().c_str());
		float padding = 3.5f;

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - padding);

		UI::Style::PushStyleFrom(ImGuiCol_Button, ImGuiCol_MenuBarBg);
		if (ImGui::Button(assetPath.string().c_str()))
		{
			currentDirectory = assetPath;
		}
		UI::Style::PopStyle();

		for (const auto& part : relativePath)
		{
			built /= part;
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - (padding + 1.f));
			ImGui::Text(">");
			ImGui::SameLine();

			if (built == currentDirectory)
			{
				ImGui::Text(part.string().c_str());
			}
			else
			{

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - padding);
				UI::Style::PushStyleFrom(ImGuiCol_Button, ImGuiCol_MenuBarBg);
				if (ImGui::Button(part.string().c_str()))
				{
					currentDirectory = built;
				}
				UI::Style::PopStyle();
			}
		}
	}
	else
	{
		ImGui::Text(assetPath.string().c_str());
		ImGui::SameLine();
		ImGui::Text(">");
	}
}

void UI::ContentBrowserPanel::DrawFileColumnsInCurrentFolder()
{
	float padding = 16.f;
	float cellSize = iconSize + padding;

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);

	if (columnCount < 1)
		columnCount = 1;

	ImGui::Columns(columnCount, nullptr, false);

	for (auto& content : std::filesystem::directory_iterator(currentDirectory))
	{
		auto& path = content.path();
		auto relativePath = std::filesystem::relative(path, assetPath);
		std::string fileName = relativePath.filename().string();

		if (content.is_directory())
			continue;

		EFileType type = GetFileType(path);

		ImTextureID icon;
		EDragAndDropType dragSource = EDragAndDropType::UNDEFINED;

		switch (type)
		{
		case EFileType::TEXTURE:
			dragSource = EDragAndDropType::TEXTURE_FILE;
			icon = (ImTextureID)textureFileTextureID;
			break;
		case EFileType::MESH:
			dragSource = EDragAndDropType::FILE3D;
			icon = (ImTextureID)meshFileTextureID;
			break;
		case EFileType::SOUND:
			dragSource = EDragAndDropType::SOUND_FILE;
			icon = (ImTextureID)soundFileTextureID;
			break;
		case EFileType::MATERIAL:
			dragSource = EDragAndDropType::MATERIAL_FILE;
			icon = (ImTextureID)materialFileTextureID;
			break;
		case EFileType::FONT:
			dragSource = EDragAndDropType::FONT_FILE;
			icon = (ImTextureID)fontFileTextureID;
			break;
		case EFileType::SCENE:
			dragSource = EDragAndDropType::SCENE_FILE;
			icon = (ImTextureID)sceneFileTextureID;
			break;
		case EFileType::PREFAB:
			dragSource = EDragAndDropType::PREFAB_FILE;
			icon = (ImTextureID)prefabFileTextureID;
			break;
		default:
			icon = (ImTextureID)commonFileTextureID;
			break;
		}


		float padding = (ImGui::GetColumnWidth() - iconSize) * 0.5f;

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding);
		ImGui::ImageButton(fileName.c_str(), icon, { iconSize, iconSize });
		if (icon == sceneFileTextureID)
		{
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
			{
				std::filesystem::path sceneNamePath = fileName.c_str();
				UIEditor::engine.ChangeScene(sceneNamePath.stem().string());
				UIEditor::engine.GetRenderer()->SetSelectedGameObject(nullptr);
			}
		}
		else if (icon == materialFileTextureID)
		{
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
			{
				std::filesystem::path file = fileName;
				UIEditor::materialToDisplay = MaterialManager::GetInstance().GetMaterial(file.stem().string());
				UIEditor::requestMaterialEditor = true;
			}
		}

		if (dragSource != EDragAndDropType::UNDEFINED)
		{
			switch (dragSource)
			{
			case EDragAndDropType::TEXTURE_FILE:
			case EDragAndDropType::FILE3D:
			case EDragAndDropType::FONT_FILE:
			case EDragAndDropType::PREFAB_FILE:
			case EDragAndDropType::MATERIAL_FILE:
				if (ImGui::BeginDragDropSource())
				{
					std::string name = fileName;
					ImGui::SetDragDropPayload(UI::ToLabel(dragSource), name.data(), name.size());
					ImGui::Text(name.c_str());
					ImGui::EndDragDropSource();
				}
				break;
			}
			
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (content.is_directory())
				currentDirectory /= path.filename();
		}

		float textWidth = ImGui::CalcTextSize(fileName.c_str()).x;
		padding = (ImGui::GetColumnWidth() - textWidth) * 0.5f;

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding);
		ImGui::Text(fileName.c_str());
		ImGui::NextColumn();
	}

	ImGui::Columns(1);
}


UI::EFileType UI::ContentBrowserPanel::GetFileType(const std::filesystem::path& path)
{
	std::string ext = path.extension().string();

	static const std::unordered_set<std::string> imageExts = 
	{
		".png", ".jpg", ".jpeg", ".hdr"
	};
	static const std::unordered_set<std::string> meshExts = 
	{
		".obj", ".fbx", ".gltf", ".glb"
	};
	static const std::unordered_set<std::string> soundExts =
	{
		".mp3", ".wav"
	};
	static const std::unordered_set<std::string> fontExts =
	{
		".ttf", ".otf"
	};
	static const std::unordered_set<std::string> materialExts =
	{
		".mat"
	};
	static const std::unordered_set<std::string> sceneExts =
	{
		".scene"
	};
	static const std::unordered_set<std::string> prefabExts =
	{
		".prefab"
	};

	if (imageExts.count(ext))
		return EFileType::TEXTURE;
	else if (meshExts.count(ext))
		return EFileType::MESH;
	else if (soundExts.count(ext))
		return EFileType::SOUND;
	else if (materialExts.count(ext))
		return EFileType::MATERIAL;
	else if (fontExts.count(ext))
		return EFileType::FONT;
	else if (sceneExts.count(ext))
		return EFileType::SCENE;
	else if (prefabExts.count(ext))
		return EFileType::PREFAB;

	return EFileType::OTHER;
}