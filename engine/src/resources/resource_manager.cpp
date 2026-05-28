#include "resources/resource_manager.h"
#include "components/mesh_renderer.h"

ResourceManager& ResourceManager::GetInstance()
{
	static ResourceManager resoureManager;
	return resoureManager;
}

std::filesystem::path ResourceManager::FindFile(const std::filesystem::path& root, const std::string& filename)
{
	for (const auto& entry : std::filesystem::recursive_directory_iterator(root))
	{
		if (entry.is_regular_file() &&
			entry.path().filename() == filename)
		{
			return entry.path();
		}
	}

	return "";
}

void ResourceManager::Delete(std::string name)
{
	auto it = resources.find(name);
	if (it != resources.end())
	{
		delete it->second;
		resources.erase(name);
	}
}

void ResourceManager::Clear()
{
	for (auto it = resources.begin(); it != resources.end(); ++it)
	{
		delete it->second;
	}
	resources.clear();
}

bool ResourceManager::IsMeshFormat(std::string format)
{
	return format == ".gltf" || format == ".glb";
}

bool ResourceManager::IsTextureFormat(std::string format)
{
	return format == ".png" || format == ".jpeg" || format == ".jpg";
}

bool ResourceManager::IsSoundFormat(std::string format)
{
	return format == ".mp3";
}