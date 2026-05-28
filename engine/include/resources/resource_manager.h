#pragma once

#include <string>
#include <unordered_map>
#include <concepts>
#include <mutex>
#include "iresource.h"
#include "IRenderInterface.h"
#include "scenes/scene.h"
#include "../utilities/macro.h"

class ENIGMA_API ResourceManager
{
public:
	static ResourceManager& GetInstance();

	template<typename T>
	requires std::derived_from<T, IResource>
	std::vector<T*> LoadAllResourcesOfType(std::string pathName, EnigmaRHI::IRenderInterface* rhi, const std::string root = "assets");

	template<typename T>
	requires std::derived_from<T, IResource>
	T* Create(const std::string filename, EnigmaRHI::IRenderInterface* rhi, const std::string root = "assets");

	template<typename T>
	requires std::derived_from<T, IResource>
	T* Get(std::string name);

	void Delete(std::string name);
	void Clear();
	std::filesystem::path FindFile(const std::filesystem::path& root, const std::string& filename);

private:
	std::unordered_map<std::string, IResource*> resources;
	std::mutex mut;

	ResourceManager() = default;
	~ResourceManager() = default;

	bool IsMeshFormat(std::string format);
	bool IsTextureFormat(std::string format);
	bool IsSoundFormat(std::string format);
};

#include "resource_manager.inl"