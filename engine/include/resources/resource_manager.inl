#include "debug/log.h"
#include "multithreading/async.h"
#include "resources/model.h"
#include "resources/texture.h"
#include "sound/sound.h"
#include <filesystem>
#include <vector>
#include <string>

template<typename T>
requires std::derived_from<T, IResource>
T* ResourceManager::Create(const std::string filename, EnigmaRHI::IRenderInterface* rhi, const std::string root)
{
	{
		std::lock_guard<std::mutex> lock(mut);
		auto it = resources.find(filename);
		if (it != resources.end())
		{
			Debug::LogWarning("Create resource already exist");
			return dynamic_cast<T*>(it->second);
		}
	}

	IResource* resource = new T();
	if (resource)
	{

		resource->Load(FindFile(root, filename), rhi);
		{
			std::lock_guard<std::mutex> lock(mut);
			resources.insert({ filename, resource });
		}
		return dynamic_cast<T*>(resource);
	}
	Debug::LogError("Create resource failed");
	return nullptr;
}

template<typename T>
requires std::derived_from<T, IResource>
T* ResourceManager::Get(std::string name)
{
	auto it = resources.find(name);
	if (it != resources.end())
	{
		return dynamic_cast<T*>(it->second);
	}
	return nullptr;
}

template<typename T>
requires std::derived_from<T, IResource>
inline std::vector<T*> ResourceManager::LoadAllResourcesOfType(std::string pathName, EnigmaRHI::IRenderInterface* rhi, const std::string root)
{
	std::vector<T*> resourcesType;
	for (const auto file : std::filesystem::recursive_directory_iterator(pathName))
	{
		if (!file.is_directory())
		{
			std::string format = file.path().extension().string();

			if ((typeid(T) == typeid(Model) && !IsMeshFormat(format)) ||
				(typeid(T) == typeid(Texture) && !IsTextureFormat(format)) ||
				(typeid(T) == typeid(Sound) && !IsSoundFormat(format)))
			{
				continue;
			}

			T* resource = ResourceManager::GetInstance().Create<T>(file.path().filename().string(), rhi, root);
			resourcesType.push_back(resource);
		}
	}

	return resourcesType;
}