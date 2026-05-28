#include "components/material_manager.h"
#include "serialization/serializer.h"

MaterialManager& MaterialManager::GetInstance()
{
	static MaterialManager materialManager;
	return materialManager;
}

Material* MaterialManager::AddMaterial(std::string name)
{
	Material mat;
	mat.name = name;
	materials.insert({ name, mat });
	Material* m = &materials.at(name);
	Serializer::GetInstance().SerializeMaterial(m);
	return m;
}

Material* MaterialManager::AddMaterial(Material* mat)
{
	Material matCopy = *mat;
	materials.insert({ mat->name, matCopy });
	Material* m = &materials.at(mat->name);
	Serializer::GetInstance().SerializeMaterial(m);
	return m;
}

Material* MaterialManager::GetMaterial(std::string name)
{
	if (materials.count(name) == 0)
	{
		Material* mat = AddMaterial(name);
		Serializer::GetInstance().DeserializeMaterial(mat);
	}

	return &materials.at(name);
}

void MaterialManager::SaveAllMaterials()
{
	Serializer& serializer = Serializer::GetInstance();
	for (auto& [name, mat] : materials)
	{
		serializer.SerializeMaterial(&mat);
	}
}