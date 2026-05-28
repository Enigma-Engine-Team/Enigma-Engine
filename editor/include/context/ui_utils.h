#pragma once

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include "emath/emath.h"
#include "gameobject/gameobject.h"
#include "engine/engine.h"

namespace UI
{
	enum class EDragAndDropType
	{
		UNDEFINED,
		GAMEOBJECT,
		TEXTURE_FILE,
		MATERIAL_FILE,
		SCENE_FILE,
		FILE3D,
		SOUND_FILE,
		FONT_FILE,
		PREFAB_FILE
	};

	bool CreateVector3Control(const char* label, Math::Vector3D* vec, bool activeLinkButton = false);
	bool CreateVector2Control(const char* label, Math::Vector2D* vec);
	void CreateCheckboxImage(const char* label, bool* boolean, unsigned int textureTrue, unsigned int textureFalse);
	void SetVector3ControlWidth(float itemWidth, float minWindowWidth, float minItemWidth, float windowWidth);
	bool TreeNodeImage(const char* text, unsigned int textureID, int flags);
	bool TreeNodeImage(GameObject* obj, unsigned int textureID, int flags);
	bool CollapsingHeaderImage(const char* name, unsigned int textureID, int flags);
	void ImageText(const char* text, unsigned int textureID);
	void ImageTextButton(const char* text, unsigned int textureID, Math::Vector2D size);
	bool RecursiveTreeNode(GameObject* currentGameObject, unsigned int textureID, int flags);
	void DrawNodeBackground(ImVec4 color);
	bool RecursiveTreeNode(std::string path, unsigned int textureID, int flags, std::filesystem::path& currentDirectory);
	const char* ToLabel(EDragAndDropType type);
}