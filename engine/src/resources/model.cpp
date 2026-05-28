#include "debug/log.h"
#include "emath/emath.h"
#include "components/mesh_renderer.h"
#include "resources/model.h"
#include <string>
#include "resources/resource_manager.h"

void Model::InstantiateIntoScene(Model* model, GameObject* parent, Scene* scene)
{
    const auto& subMeshes = model->GetSubMeshes();

    auto ApplyTransform = [](GameObject* go, const Math::Matrix4x4& t)
    {
        go->transform.position = { t.m[12], t.m[13], t.m[14] };

        Math::Vector3D col0 = { t.m[0], t.m[1], t.m[2] };
        Math::Vector3D col1 = { t.m[4], t.m[5], t.m[6] };
        Math::Vector3D col2 = { t.m[8], t.m[9], t.m[10] };
        go->transform.scale = { col0.Magnitude(), col1.Magnitude(), col2.Magnitude() };

        Math::Matrix3x3 rotMat;
        rotMat.m[0] = t.m[0] / go->transform.scale.x;
        rotMat.m[1] = t.m[1] / go->transform.scale.x;
        rotMat.m[2] = t.m[2] / go->transform.scale.x;
        rotMat.m[3] = t.m[4] / go->transform.scale.y;
        rotMat.m[4] = t.m[5] / go->transform.scale.y;
        rotMat.m[5] = t.m[6] / go->transform.scale.y;
        rotMat.m[6] = t.m[8] / go->transform.scale.z;
        rotMat.m[7] = t.m[9] / go->transform.scale.z;
        rotMat.m[8] = t.m[10] / go->transform.scale.z;
        go->transform.rotation = Math::Quaternion::FromRotationMatrix(rotMat);
    };

    if (subMeshes.size() == 1)
    {
        MeshRenderer* mr = parent->AddComponent<MeshRenderer>();
        mr->SetMesh(subMeshes[0], 0, model->GetName());
        ApplyTransform(parent, model->GetLocalTransformForMesh(0));
        mr->SetAABB(model->GetAABBForGameObject(0));

        if (Material* mat = model->GetMaterialForMesh(0))
            mr->SetMaterial(mat);
        return;
    }

    for (int i = 0; i < (int)subMeshes.size(); i++)
    {
        GameObject* child = scene->AddGameObject(model->GetNameForGameObject(i));
        parent->AddChild(child);

        MeshRenderer* mr = child->AddComponent<MeshRenderer>();
        mr->SetMesh(subMeshes[i], i, model->GetName());
        ApplyTransform(child, model->GetLocalTransformForMesh(i));
        mr->SetAABB(model->GetAABBForGameObject(i));

        if (Material* mat = model->GetMaterialForMesh(i))
            mr->SetMaterial(mat);
    }
}

void Model::Load(const std::filesystem::path& filepath, EnigmaRHI::IRenderInterface* rhi)
{
    filename = filepath.filename().string();
    LoadModel(filepath.string(), rhi);
}

void Model::CreateGPUResources(EnigmaRHI::IRenderInterface* rhi)
{
    for (int i = 0 ; i < meshEntries.size() ; i++)
    {
        meshEntries[i].mesh->CreateIndexBuffer(rhi);
        meshEntries[i].mesh->CreateVertexBuffer(rhi);
    }

    for (int i = 0; i < meshEntries.size(); i++)
    {
        meshEntries[i].material.CreateMaterialData(rhi);
    }
}

std::vector<Mesh*> Model::GetSubMeshes()
{
    std::vector<Mesh*> result;
    for (const auto& e : meshEntries) result.push_back(e.mesh);
    return result;
}

Material* Model::GetMaterialForMesh(int index)
{
    if (index >= 0 && index < meshEntries.size())
        return &meshEntries[index].material;
    return nullptr;
}

Math::Matrix4x4 Model::GetLocalTransformForMesh(int index)
{
    if (index >= 0 && index < meshEntries.size())
        return meshEntries[index].localTransform;
    return Math::Matrix4x4::Identity;
}

std::string Model::GetNameForGameObject(int index)
{
    if (index >= 0 && index < meshEntries.size())
        return meshEntries[index].name;
    return "";
}

Math::AABB Model::GetAABBForGameObject(int index)
{
    if (index >= 0 && index < meshEntries.size())
        return meshEntries[index].aabb;
    return { Math::Vector3D::Zero, Math::Vector3D::Zero };
}

void Model::LoadModel(std::string path, EnigmaRHI::IRenderInterface* rhi)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_FlipUVs | aiProcess_GenBoundingBoxes);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::string debug = importer.GetErrorString();
        Debug::LogError("Error loading model: " + debug);
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));

    ProcessNode(scene->mRootNode, scene, aiMatrix4x4(), rhi);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransform, EnigmaRHI::IRenderInterface* rhi)
{
    aiMatrix4x4 globalTransform = parentTransform * node->mTransformation;

    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        if (!textureLoaded)
        {
            if (texturePath == "")
            {
                texturePath = GetTextureDirectory(scene, mesh, directory);
            }

            if (texturePath != "")
            {
                LoadAllTextures(rhi);
                textureLoaded = true;
            }
        }

        Material meshMaterial;

        //if (material)
        //{
        //    meshMaterial = *material;
        //}

        Mesh* processedMesh = ProcessMesh(mesh, scene, meshMaterial);

        aiVector3D min = mesh->mAABB.mMin;
        aiVector3D max = mesh->mAABB.mMax;

        Math::AABB myAABB
        {
            .minCorner = Math::Vector3D(min.x, min.y, min.z),
            .maxCorner = Math::Vector3D(max.x, max.y, max.z),
        };

        MeshEntry entry;
        entry.name = node->mName.C_Str();
        entry.mesh = processedMesh;
        entry.material = meshMaterial;
        entry.localTransform = AiMatToMath(globalTransform);
        entry.aabb = myAABB;

        meshEntries.push_back(entry);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
        ProcessNode(node->mChildren[i], scene, globalTransform, rhi);
}

Mesh* Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, Material& outMaterial)
{
    Mesh* tMesh = new Mesh();

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

        if (mesh->HasNormals())
        {
            vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
        }

        if (mesh->HasTextureCoords(0))
        {
            vertex.texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        }

        tMesh->vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) 
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            tMesh->indices.push_back(face.mIndices[j]);
    }

    if (!material)
    {
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* aiMaterial = scene->mMaterials[mesh->mMaterialIndex];

            MaterialGPUData mat{};

            aiColor4D color;
            aiColor4D emissiveColor;
            float metallic = 0.f, roughness = 0.f, clearCoatFactor = 0.f, clearCoatRoughFactor = 0.f, emissiveIntensity = 0.f, opacity = 1.f, normalScale = 1.f;

            if (AI_SUCCESS == aiGetMaterialColor(aiMaterial, AI_MATKEY_BASE_COLOR, &color))
                mat.albedoColor = { color.r, color.g, color.b, color.a };
            else if (AI_SUCCESS == aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_DIFFUSE, &color))
                mat.albedoColor = { color.r, color.g, color.b, color.a };

            if (AI_SUCCESS == aiGetMaterialFloat(aiMaterial, AI_MATKEY_GLTF_TEXTURE_SCALE(aiTextureType_NORMALS, 0), &normalScale))
                mat.normalStrength = normalScale;

            if (AI_SUCCESS == aiGetMaterialFloat(aiMaterial, AI_MATKEY_EMISSIVE_INTENSITY, &emissiveIntensity))
                mat.emissiveColorIntensity = Math::Vector4D(mat.emissiveColorIntensity, emissiveIntensity);

            if (AI_SUCCESS == aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_EMISSIVE, &emissiveColor))
                mat.emissiveColorIntensity = Math::Vector4D(emissiveColor.r, emissiveColor.g, emissiveColor.b, mat.emissiveColorIntensity.w);

            aiString alphaMode;
            if (AI_SUCCESS == aiGetMaterialString(aiMaterial, AI_MATKEY_GLTF_ALPHAMODE, &alphaMode))
            {
                std::string mode = alphaMode.C_Str();
                if (mode == "BLEND")
                    outMaterial.forceTransparent = true;
                else if (mode == "OPAQUE")
                    mat.albedoColor.w = 1.0f;
            }

            if (AI_SUCCESS == aiGetMaterialFloat(aiMaterial, AI_MATKEY_METALLIC_FACTOR, &metallic))
                mat.metallicValue = metallic;

            if (AI_SUCCESS == aiGetMaterialFloat(aiMaterial, AI_MATKEY_ROUGHNESS_FACTOR, &roughness))
                mat.roughnessValue = roughness;

            if (AI_SUCCESS == aiGetMaterialFloat(aiMaterial, AI_MATKEY_CLEARCOAT_FACTOR, &clearCoatFactor))
                mat.clearCoatFactor = clearCoatFactor;

            if (AI_SUCCESS == aiGetMaterialFloat(aiMaterial, AI_MATKEY_CLEARCOAT_ROUGHNESS_FACTOR, &clearCoatRoughFactor))
                mat.clearCoatRoughFactor = clearCoatRoughFactor;

            if (AI_SUCCESS == aiGetMaterialFloat(aiMaterial, AI_MATKEY_OPACITY, &opacity))
            {
                mat.albedoColor.w *= opacity;
                if (opacity < 1.0f)
                    outMaterial.forceTransparent = true;
            }

            aiString name;
            aiMaterial->Get(AI_MATKEY_NAME, name);

            std::string matName = name.C_Str();

            outMaterial.SetMaterialGPUData(mat);

            //albedo
            if (aiMaterial->GetTextureCount(aiTextureType_BASE_COLOR) > 0)
            {
                outMaterial.useAlbedoTexture = true;
                outMaterial.albedoTexture = GetMaterialTexture(aiMaterial, aiTextureType_BASE_COLOR);
            }
            else if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
            {
                outMaterial.useAlbedoTexture = true;
                outMaterial.albedoTexture = GetMaterialTexture(aiMaterial, aiTextureType_DIFFUSE);
            }

            // metallicRoughness
            if (aiMaterial->GetTextureCount(aiTextureType_UNKNOWN)) {
                outMaterial.useMetallicRoughnessTexture = true;
                outMaterial.metallicRoughnessTexture = GetMaterialTexture(aiMaterial, aiTextureType_UNKNOWN);
            }
            else
            {
                if (aiMaterial->GetTextureCount(aiTextureType_METALNESS))
                {
                    outMaterial.useMetallicTexture = true;
                    outMaterial.metallicTexture = GetMaterialTexture(aiMaterial, aiTextureType_METALNESS);
                }
                if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS))
                {
                    outMaterial.useRoughnessTexture = true;
                    outMaterial.roughnessTexture = GetMaterialTexture(aiMaterial, aiTextureType_DIFFUSE_ROUGHNESS);
                }
            }

            // normal
            if (aiMaterial->GetTextureCount(aiTextureType_NORMALS)) {
                outMaterial.useNormalTexture = true;
                outMaterial.normalTexture = GetMaterialTexture(aiMaterial, aiTextureType_NORMALS);
            }

            // ambient occlusion
            if (aiMaterial->GetTextureCount(aiTextureType_LIGHTMAP))
            {
                outMaterial.useAOTexture = true;
                outMaterial.aoTexture = GetMaterialTexture(aiMaterial, aiTextureType_LIGHTMAP);
            }

            // Clear coat factor texture
            if (aiMaterial->GetTextureCount(aiTextureType_CLEARCOAT) > 0)
            {
                aiString path;
                aiMaterial->GetTexture(aiTextureType_CLEARCOAT, 0, &path);
                std::string p = path.C_Str();
                if (!p.empty())
                {
                    outMaterial.clearCoatTexture = GetMaterialTexture(aiMaterial, aiTextureType_CLEARCOAT, 0);
                    outMaterial.useClearCoatTexture = true;
                }
            }
            if (aiMaterial->GetTextureCount(aiTextureType_CLEARCOAT) > 1)
            {
                aiString path;
                aiMaterial->GetTexture(aiTextureType_CLEARCOAT, 1, &path);
                std::string p = path.C_Str();
                if (!p.empty())
                {
                    outMaterial.clearCoatRoughTexture = GetMaterialTexture(aiMaterial, aiTextureType_CLEARCOAT, 1);
                    outMaterial.useClearCoatRoughTexture = true;
                }
            }
            if (aiMaterial->GetTextureCount(aiTextureType_CLEARCOAT) > 2)
            {
                aiString path;
                aiMaterial->GetTexture(aiTextureType_CLEARCOAT, 2, &path);
                std::string p = path.C_Str();
                if (!p.empty())
                {
                    outMaterial.clearCoatNormalTexture = GetMaterialTexture(aiMaterial, aiTextureType_CLEARCOAT, 2);
                    outMaterial.useClearCoatNormalTexture = true;
                }
            }

            //emissive
            if (aiMaterial->GetTextureCount(aiTextureType_EMISSIVE) > 0)
            {
                outMaterial.emissiveTexture = GetMaterialTexture(aiMaterial, aiTextureType_EMISSIVE);
                outMaterial.useEmissiveTexture = true;
            }
        }
    }

    return tMesh;
}

Texture Model::GetMaterialTexture(aiMaterial* material, aiTextureType type, unsigned int textureIndex)
{
    aiString path;
    material->GetTexture(type, textureIndex, &path);

    std::string fileName = std::filesystem::path(path.C_Str()).filename().string();

    Texture* t = ResourceManager::GetInstance().Get<Texture>(fileName);

    if (!t)
        return Texture();

    if (type == aiTextureType_DIFFUSE || type == aiTextureType_BASE_COLOR)
    {
        EnigmaRHI::EImageFormat imageFormat = t->GetImageFormat();

        if (imageFormat == EnigmaRHI::EImageFormat::RGB8)
        {
            t->OverrideInternalFormat(EnigmaRHI::EImageFormat::sRGB);
        }
        else if (imageFormat == EnigmaRHI::EImageFormat::RGBA8)
        {
            t->OverrideInternalFormat(EnigmaRHI::EImageFormat::sRGBA);
        }
    }

    return *t;
}

void Model::LoadAllTextures(EnigmaRHI::IRenderInterface* rhi)
{
    ResourceManager::GetInstance().LoadAllResourcesOfType<Texture>(texturePath, rhi);
}

Math::Matrix4x4 Model::AiMatToMath(const aiMatrix4x4& m)
{
    return Math::Matrix4x4(
        m.a1, m.b1, m.c1, m.d1,
        m.a2, m.b2, m.c2, m.d2,
        m.a3, m.b3, m.c3, m.d3,
        m.a4, m.b4, m.c4, m.d4
    );
}

bool Model::MeshHasTextures(const aiScene* scene, const aiMesh* mesh)
{
    if (scene->mNumMaterials == 0)
        return false;

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    for (unsigned int i = 1; i <= AI_TEXTURE_TYPE_MAX; i++)
    {
        aiTextureType type = static_cast<aiTextureType>(i);

        if (material->GetTextureCount(type) > 0)
            return true;
    }

    return false;
}

std::string Model::GetTextureDirectory(const aiScene* scene, const aiMesh* mesh, std::string modelPath)
{
    if (!scene || !mesh || scene->mNumMaterials == 0)
        return "";

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    aiString aiPath;

    for (unsigned int i = 0; i <= AI_TEXTURE_TYPE_MAX; i++)
    {
        aiTextureType type = static_cast<aiTextureType>(i);

        if (material->GetTexture(type, 0, &aiPath) == AI_SUCCESS)
        {
            std::filesystem::path fullPath(aiPath.C_Str());
            std::string dirName = fullPath.parent_path().string();
            std::filesystem::path finalPath = std::filesystem::path(modelPath).parent_path();

            if (!dirName.empty())
            {
                finalPath /= dirName;
            }

            return finalPath.string();
        }
    }
    return "";
}