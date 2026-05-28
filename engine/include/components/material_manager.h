#pragma once

#include <unordered_map>
#include "material.h"
#include <string>
#include "../utilities/macro.h"

class ENIGMA_API MaterialManager
{
public:
	static MaterialManager& GetInstance();
	~MaterialManager() = default;
	Material* AddMaterial(std::string name);
	Material* AddMaterial(Material* mat);
	Material* GetMaterial(std::string name);
	void SaveAllMaterials();

private:
	std::unordered_map<std::string, Material> materials;
	MaterialManager() = default;
};