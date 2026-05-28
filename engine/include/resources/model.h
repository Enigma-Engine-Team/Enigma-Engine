#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/pbrmaterial.h>
#include "resources/iresource.h"
#include "resources/texture.h"
#include "components/material.h"
#include "gameobject/gameobject.h"
#include "scenes/scene.h"
#include <filesystem>
#include "mesh.h"
#include "../utilities/macro.h"
#include <map>

class ENIGMA_API Model : public IResource
{
public:
	virtual ~Model() override {};

	static void InstantiateIntoScene(Model* model, GameObject* parent, Scene* scene);

	void Load(const std::filesystem::path& filepath, EnigmaRHI::IRenderInterface* rhi) override;
	void CreateGPUResources(EnigmaRHI::IRenderInterface* rhi);

	std::vector<Mesh*> GetSubMeshes();
	Material* GetMaterialForMesh(int index);
	Math::Matrix4x4 GetLocalTransformForMesh(int index);
	std::string GetNameForGameObject(int index);
	Math::AABB GetAABBForGameObject(int index);


private:
	void LoadModel(std::string path, EnigmaRHI::IRenderInterface* rhi);
	void ProcessNode(aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransform, EnigmaRHI::IRenderInterface* rhi);
	Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene, Material& outMaterial);
	Texture GetMaterialTexture(aiMaterial* material, aiTextureType type, unsigned int textureIndex = 0);
	void LoadAllTextures(EnigmaRHI::IRenderInterface* rhi);
	Math::Matrix4x4 AiMatToMath(const aiMatrix4x4& m);
	bool MeshHasTextures(const aiScene* scene, const aiMesh* mesh);
	std::string GetTextureDirectory(const aiScene* scene, const aiMesh* mesh, std::string modelPath);

	std::string directory = "";
	std::string texturePath = "";
	std::shared_ptr<Material> material;
	bool textureLoaded = false;
	bool hasTexture = false;

	struct MeshEntry
	{
		std::string name;
		Mesh* mesh;
		Material material;
		Math::Matrix4x4 localTransform;
		Math::AABB aabb;
	};

	std::vector<MeshEntry> meshEntries;
};