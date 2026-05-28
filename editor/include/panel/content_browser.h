#pragma once

#include "context/panel.h"
#include "IImage.h"
#include <iostream>
#include <filesystem>

namespace UI
{
	enum class EFileType
	{ 
		TEXTURE, 
		MESH, 
		SOUND,
		MATERIAL,
		FONT,
		SCENE,
		PREFAB,
		OTHER
	};

	class ContentBrowserPanel : public IUIPanel
	{
	public:
		ContentBrowserPanel();

		void Draw() override;

	private:

		void DrawFolderListPath();
		void DrawFileColumnsInCurrentFolder();

		EFileType GetFileType(const std::filesystem::path& path);

		const float iconSize = 128.f;
		std::filesystem::path assetPath = "assets";
		std::filesystem::path currentDirectory = "";

		unsigned int folderTextureID = 0;
		unsigned int commonFileTextureID = 0;
		unsigned int meshFileTextureID = 0;
		unsigned int soundFileTextureID = 0;
		unsigned int textureFileTextureID = 0;
		unsigned int materialFileTextureID = 0;
		unsigned int fontFileTextureID = 0;
		unsigned int sceneFileTextureID = 0;
		unsigned int prefabFileTextureID = 0;
	};
}