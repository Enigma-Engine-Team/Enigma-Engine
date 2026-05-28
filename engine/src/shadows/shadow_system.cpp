#include "shadows/shadow_system.h"
#include "shadows/directional_map.h"
#include "shadows/spot_map.h"
#include "shadows/point_map.h"
#include "components/mesh_renderer.h"
#include "utilities/shader_common.h"

// ──────────────────────────────────────────────
// Lifecycle
// ──────────────────────────────────────────────

void ShadowSystem::Create(EnigmaRHI::IRenderInterface* rhi)
{
    directionalShadowVert = rhi->InstantiateShaderModule();
    directionalShadowFrag = rhi->InstantiateShaderModule();
    directionalShadowGeom = rhi->InstantiateShaderModule();
    directionalShadowVert->Create("shaders/shadow_depth.vert", EnigmaRHI::EShaderType::VERTEX);
    directionalShadowFrag->Create("shaders/shadow_depth.frag", EnigmaRHI::EShaderType::FRAGMENT);
    directionalShadowGeom->Create("shaders/shadow_depth_directional.geo", EnigmaRHI::EShaderType::GEOMETRY);

    EnigmaRHI::GraphicsPipeline depthDesc
    {
        .cullMode = EnigmaRHI::ECullMode::DISABLED,
        .frontFaceMode = EnigmaRHI::EFrontFaceMode::COUNTER_CLOCK_WISE,
        .depthTestEnable = true,
        .depthWriteEnable = true,
    };

    directionalShadowPipeline = rhi->InstantiatePipeline();
    directionalShadowPipeline->Create(depthDesc, directionalShadowVert, directionalShadowFrag, directionalShadowGeom);

    // ── Spot ─────────────────────────────────────────────────────────────
    spotShadowVert = rhi->InstantiateShaderModule();
    spotShadowVert->Create("shaders/shadow_depth_spot.vert", EnigmaRHI::EShaderType::VERTEX);

    spotShadowPipeline = rhi->InstantiatePipeline();
    // Même frag (vide), pas de geo shader
    spotShadowPipeline->Create(depthDesc, spotShadowVert, directionalShadowFrag);

    // ── Point ─────────────────────────────────────────────────────────────
    pointShadowVert = rhi->InstantiateShaderModule();
    pointShadowFrag = rhi->InstantiateShaderModule();
    pointShadowGeom = rhi->InstantiateShaderModule();
    pointShadowVert->Create("shaders/shadow_depth_point.vert", EnigmaRHI::EShaderType::VERTEX);
    pointShadowFrag->Create("shaders/shadow_depth_point.frag", EnigmaRHI::EShaderType::FRAGMENT);
    pointShadowGeom->Create("shaders/shadow_depth_point.geo", EnigmaRHI::EShaderType::GEOMETRY);

    pointShadowPipeline = rhi->InstantiatePipeline();
    pointShadowPipeline->Create(depthDesc, pointShadowVert, pointShadowFrag, pointShadowGeom);
}

void ShadowSystem::Destroy(EnigmaRHI::IRenderInterface* rhi)
{
    for (IShadowMap* sm : shadowMaps)
    {
        sm->Destroy(rhi);
        delete sm;
    }
    shadowMaps.clear();

    rhi->DeleteShaderModule(directionalShadowVert);
    rhi->DeleteShaderModule(directionalShadowFrag);
    rhi->DeleteShaderModule(directionalShadowGeom);

    directionalShadowPipeline->Destroy();
    rhi->DeletePipeline(directionalShadowPipeline);
}

IShadowMap* ShadowSystem::CreateShadowMap(ELightType lightType, EnigmaRHI::IRenderInterface* rhi)
{
    switch (lightType)
    {
    case ELightType::DIRECTIONAL:
    {
        DirectionalMap* map = new DirectionalMap();
        map->Create(rhi);
        return map;
    }
    case ELightType::SPOT:
    {
        SpotMap* map = new SpotMap();
        map->Create(rhi);
        return map;
    }
    case ELightType::POINT:
    {
        PointMap* map = new PointMap();
        map->Create(rhi);
        return map;
    }
    default:
        return nullptr;
    }
}

void ShadowSystem::RenderShadows(Scene* currentScene, LightManager* lightMng,
    EnigmaRHI::ICommandBuffer& cmd, EnigmaRHI::IRenderPass* renderPass,
    EnigmaRHI::IRenderInterface* rhi, EnigmaRHI::IDevice* device,
    EnigmaRHI::IVertexInput* meshVertexMode,
    EnigmaRHI::IDescriptor* lightningDescriptor,
    const CameraProxy& camera)
{
    std::vector<Light*> lights = lightMng->GetLightsInScene();

    // Collecter les IDs actifs
    std::unordered_set<uint32_t> activeLightIDs;
    for (Light* l : lights)
        activeLightIDs.insert(reinterpret_cast<uint32_t>(l));

    // Détruire les shadow maps dont la light n'existe plus
    for (auto it = shadowMapCache.begin(); it != shadowMapCache.end(); )
    {
        if (activeLightIDs.find(it->first) == activeLightIDs.end())
        {
            it->second->Destroy(rhi);
            delete it->second;
            it = shadowMapCache.erase(it);
        }
        else ++it;
    }

    // Render
    for (Light* light : lights)
    {
        uint32_t id = reinterpret_cast<uint32_t>(light);
        ELightType lightType = static_cast<ELightType>(light->GetLightType());

        // Créer la shadow map si elle n'existe pas encore pour cette light
        if (shadowMapCache.find(id) == shadowMapCache.end())
            shadowMapCache[id] = CreateShadowMap(lightType, rhi);

        IShadowMap* sm = shadowMapCache[id];
        sm->UpdateLightSpaceMatrix(camera, light);

        const int res = sm->GetShadowResolution();
        EShadowMapType smType = sm->GetType();

        sm->BindShadowFBO();
        renderPass->SetViewport(0, 0, res, res);
        renderPass->ClearBuffer(EnigmaRHI::EMask::DEPTH);

        if (smType == EShadowMapType::Directional)
        {
            lightningDescriptor->BindBuffer(13, EnigmaRHI::EBufferTarget::UNIFORM_BUFFER,
                sm->GetMatricesUBO());
            cmd.BindPipeline(device, directionalShadowPipeline);
        }
        else if (smType == EShadowMapType::Spot)
        {
            cmd.BindPipeline(device, spotShadowPipeline);
        }
        else if (smType == EShadowMapType::Point)
        {
            lightningDescriptor->BindBuffer(14, EnigmaRHI::EBufferTarget::UNIFORM_BUFFER,
                sm->GetMatricesUBO());
            cmd.BindPipeline(device, pointShadowPipeline);
        }

        cmd.BindVertexInput(meshVertexMode);
        RenderScene(currentScene, cmd, rhi, sm, light);
        sm->UnbindShadowFBO();

        light->SetLightSpaceMatrix(sm->GetLightSpaceMatrix());
    }
}

void ShadowSystem::RenderScene(Scene* scene, EnigmaRHI::ICommandBuffer& cmd,
    EnigmaRHI::IRenderInterface* rhi, IShadowMap* sm, Light* light)
{
    for (GameObject* go : scene->GetGameObjects())
    {
        MeshRenderer* mr = go->GetComponent<MeshRenderer>();
        if (!mr || !mr->CastShadows()) continue;

        mr->UpdateMeshRenderData(go->transform.global, rhi);

        EShadowMapType type = sm->GetType();

        if (type == EShadowMapType::Directional)
        {
            directionalShadowPipeline->SendToGPU("model", go->transform.global.m);
        }
        else if (type == EShadowMapType::Spot)
        {
            spotShadowPipeline->SendToGPU("model", go->transform.global.m);
            spotShadowPipeline->SendToGPU("lightSpaceMatrix", sm->GetLightSpaceMatrix().m);
        }
        else if (type == EShadowMapType::Point)
        {
            auto* pm = static_cast<PointMap*>(sm);
            pointShadowPipeline->SendToGPU("model", go->transform.global.m);
            pointShadowPipeline->SendToGPU("lightPos", light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);
            pointShadowPipeline->SendToGPU("farPlane", pm->GetFarPlane());
        }

        mr->GetMesh()->Draw(cmd);
    }
}

void ShadowSystem::BindForLightning(EnigmaRHI::IPipeline* lightningPipeline,
    EnigmaRHI::IDescriptor* lightningDescriptor, const std::vector<Light*>& lights)
{
    bool hasDirectional = false;
    int spotSlot = 0;
    int pointSlot = 0;

    for (Light* light : lights)
    {
        auto it = shadowMapCache.find(reinterpret_cast<uint32_t>(light));
        if (it == shadowMapCache.end()) continue;

        IShadowMap* sm = it->second;
        EShadowMapType type = sm->GetType();

        if (type == EShadowMapType::Directional && !hasDirectional)
        {
            auto* dir = static_cast<DirectionalMap*>(sm);
            const auto& levels = dir->GetCascadeLevels();
            int cascadeCount = static_cast<int>(levels.size() + 1);

            lightningPipeline->SendToGPU("shadowFarPlane", dir->GetShadowDistance());
            lightningPipeline->SendToGPU("cascadeCount", cascadeCount);

            for (int i = 0; i < levels.size(); ++i)
            {
                lightningPipeline->SendToGPU(("cascadePlaneDistances[" + std::to_string(i) + "]").c_str(), levels[i]);
            }

            lightningPipeline->SendToGPU(("cascadePlaneDistances[" + std::to_string(levels.size()) + "]").c_str(), dir->GetShadowDistance());

            lightningDescriptor->BindImage(12, EnigmaRHI::EImageType::TYPE_2D_ARRAY,
                sm->GetDepth()->GetID());
            hasDirectional = true;
        }
        else if (type == EShadowMapType::Spot && spotSlot < MAX_SPOT_LIGHTS)
        {
            auto* spot = static_cast<SpotMap*>(sm);
            lightningPipeline->SendToGPU(
                ("spotFarPlane[" + std::to_string(spotSlot) + "]").c_str(), spot->GetFarPlane());
            lightningDescriptor->BindImage(SPOT_SHADOW_BINDING + spotSlot,
                EnigmaRHI::EImageType::TYPE_2D,
                sm->GetDepth()->GetID());
            ++spotSlot;
        }

        // Points
        else if (type == EShadowMapType::Point && pointSlot < MAX_POINT_LIGHTS)
        {
            auto* point = static_cast<PointMap*>(sm);
            lightningPipeline->SendToGPU(
                ("pointFarPlane[" + std::to_string(pointSlot) + "]").c_str(), point->GetFarPlane());
            lightningDescriptor->BindImage(POINT_SHADOW_BINDING + pointSlot,
                EnigmaRHI::EImageType::TYPE_CUBE_MAP,
                sm->GetDepth()->GetID());
            ++pointSlot;
        }
    }

    if (!hasDirectional)
        lightningPipeline->SendToGPU("cascadeCount", 0);

    lightningPipeline->SendToGPU("spotCount", spotSlot);
    lightningPipeline->SendToGPU("pointCount", pointSlot);
}