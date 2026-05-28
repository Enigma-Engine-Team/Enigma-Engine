#include "renderer/render_instance.h"
#include "components/mesh_renderer.h"
#include "renderer/light_manager.h"

void RenderInstance::GeometryPass(Scene* scene, EnigmaRHI::ICommandBuffer& cmd)
{
    cmd.BindPipeline(ctx->GetDevice(), pipelines->GetGeometryPipeline());
    gBuffer->Bind();
    gBuffer->Resize(resolution.x, resolution.y);

    renderPass->SetViewport(0, 0, (int)resolution.x, (int)resolution.y);
    renderPass->ClearColor(0.f, 0.f, 0.f, 1.f);
    renderPass->ClearBuffer(EnigmaRHI::EMask::COLOR | EnigmaRHI::EMask::DEPTH);

    cmd.BindVertexInput(pipelines->GetMeshVertexMode());
    pipelines->GetGeometryDescriptor()->BindBuffer(0, EnigmaRHI::EBufferTarget::UNIFORM_BUFFER, camera.GetDataBuffer());

    for (GameObject* go : scene->GetGameObjects())
    {
        MeshRenderer* mr = go->GetComponent<MeshRenderer>();
        if (!mr) continue;

        mr->UpdateMeshRenderData(go->transform.global, ctx->GetRHI());

        if (!mr->GetMaterial()) { Debug::LogError("Material null"); continue; }

        if (!Math::Collision::CheckFrustumAABB(mr->GetAABB(), camera.GetFrustum()))
            continue;

        bool isTransparent = mr->GetMaterial()->GetMaterialGPUData().albedoColor.w < 1.f
            || mr->GetMaterial()->forceTransparent;
        if (isTransparent) { transparentObjects.push_back(go); continue; }

        pipelines->GetGeometryDescriptor()->BindBuffer(2, EnigmaRHI::EBufferTarget::UNIFORM_BUFFER, mr->GetRenderMeshDatas());
        mr->Render(cmd, pipelines->GetGeometryDescriptor());
    }

    gBuffer->Unbind();
}

void RenderInstance::LightingPass(Scene* scene, EnigmaRHI::ICommandBuffer& cmd)
{
    auto* desc = pipelines->GetLightningDescriptor();
    cmd.BindPipeline(ctx->GetDevice(), pipelines->GetLightningPipeline());

    lightingFBO->Bind();
    lightingFBO->Resize(resolution.x, resolution.y);
    renderPass->SetViewport(0, 0, (int)resolution.x, (int)resolution.y);
    renderPass->ClearColor(0.f, 0.f, 0.f, 0.f);
    renderPass->ClearBuffer(EnigmaRHI::EMask::COLOR | EnigmaRHI::EMask::DEPTH);

    // G-Buffer
    desc->BindImage(0, EnigmaRHI::EImageType::TYPE_2D, gBuffer->GetPosition()->GetID());
    desc->BindImage(1, EnigmaRHI::EImageType::TYPE_2D, gBuffer->GetNormal()->GetID());
    desc->BindImage(2, EnigmaRHI::EImageType::TYPE_2D, gBuffer->GetAlbedoAO()->GetID());
    desc->BindImage(3, EnigmaRHI::EImageType::TYPE_2D, gBuffer->GetMetalRough()->GetID());
    desc->BindImage(10, EnigmaRHI::EImageType::TYPE_2D, gBuffer->GetClearCoatFactors()->GetID());
    desc->BindImage(11, EnigmaRHI::EImageType::TYPE_2D, gBuffer->GetClearCoatNormal()->GetID());
    desc->BindImage(14, EnigmaRHI::EImageType::TYPE_2D, gBuffer->GetEmissiveMap()->GetID());

    // Camera
    desc->BindBuffer(4, EnigmaRHI::EBufferTarget::UNIFORM_BUFFER, camera.GetDataBuffer());

    // Lights
    LightManager& lightMng = LightManager::GetInstance();
    lightMng.UpdateStorageBuffer();
    desc->BindBuffer(5, EnigmaRHI::EBufferTarget::SHADER_STORAGE_BUFFER, lightMng.GetLightStorageBuffer());

    // IBL
    auto* env = resources->GetEnvironmentSystem();
    desc->BindImage(6, EnigmaRHI::EImageType::TYPE_CUBE_MAP, env->GetIrradianceMap()->GetID());
    desc->BindImage(7, EnigmaRHI::EImageType::TYPE_CUBE_MAP, env->GetPrefilteredMapp()->GetID());
    desc->BindImage(8, EnigmaRHI::EImageType::TYPE_2D, env->GetBrdfMap()->GetID());
    desc->BindImage(9, EnigmaRHI::EImageType::TYPE_2D, env->GetSkyboxTexture()->GetID());

    resources->GetShadowSystem()->BindForLightning(pipelines->GetLightningPipeline(), desc, lightMng.GetLightsInScene());

    cmd.Draw(EnigmaRHI::EDrawMode::TRIANGLES, 3);

    // Blit depth GBuffer -> lightingFBO
    gBuffer->framebuffer->Blit(lightingFBO, (uint32_t)resolution.x, (uint32_t)resolution.y,
        EnigmaRHI::EMask::DEPTH, EnigmaRHI::EFilteringMode::NEAREST);

    lightingFBO->Bind();

    // Skybox
    cmd.BindPipeline(ctx->GetDevice(), env->envCtx->GetSkyboxPipeline());
    env->envCtx->RenderSkybox(cmd);

    ForwardPass(cmd);
}

void RenderInstance::ForwardPass(EnigmaRHI::ICommandBuffer& cmd)
{
    auto* desc = pipelines->GetGeometryDescriptor();
    auto* env = resources->GetEnvironmentSystem();

    cmd.BindVertexInput(pipelines->GetMeshVertexMode());
    desc->BindBuffer(0, EnigmaRHI::EBufferTarget::UNIFORM_BUFFER, camera.GetDataBuffer());
    desc->BindBuffer(12, EnigmaRHI::EBufferTarget::SHADER_STORAGE_BUFFER, LightManager::GetInstance().GetLightStorageBuffer());
    desc->BindImage(13, EnigmaRHI::EImageType::TYPE_CUBE_MAP, env->GetIrradianceMap()->GetID());
    desc->BindImage(14, EnigmaRHI::EImageType::TYPE_CUBE_MAP, env->GetPrefilteredMapp()->GetID());
    desc->BindImage(15, EnigmaRHI::EImageType::TYPE_2D, env->GetBrdfMap()->GetID());

    std::vector<TransparentEntry> sorted;
    sorted.reserve(transparentObjects.size());
    Math::Vector3D camPos = camera.GetPosition();
    for (GameObject* go : transparentObjects)
        sorted.push_back({ (camPos - go->transform.position).SquaredMagnitude(), go });
    std::sort(sorted.begin(), sorted.end(), [](const TransparentEntry& a, const TransparentEntry& b) { return a.distance > b.distance; });

    for (const auto& entry : sorted)
    {
        if (!entry.go) continue;
        MeshRenderer* mr = entry.go->GetComponent<MeshRenderer>();
        if (!mr) continue;

        mr->UpdateMeshRenderData(entry.go->transform.global, ctx->GetRHI());
        desc->BindBuffer(2, EnigmaRHI::EBufferTarget::UNIFORM_BUFFER, mr->GetRenderMeshDatas());

        cmd.BindPipeline(ctx->GetDevice(), pipelines->GetBackTransparentPipeline());
        mr->Render(cmd, desc);
        cmd.BindPipeline(ctx->GetDevice(), pipelines->GetFrontTransparentPipeline());
        mr->Render(cmd, desc);
    }

    transparentObjects.clear();
}

void RenderInstance::BloomPass(unsigned int srcTexture, EnigmaRHI::IRenderPass* renderPass, EnigmaRHI::IDevice* device, EnigmaRHI::ICommandBuffer& cmd)
{
    bloomPass.RenderBloomTexture(lightingFBO->GetColorAttachmentID(1), 0.005f, device, cmd);
    
    bloomFBO->Bind();
    renderPass->ClearBuffer(EnigmaRHI::EMask::COLOR | EnigmaRHI::EMask::DEPTH);

    cmd.BindVertexInput(pipelines->GetMeshVertexMode());
    cmd.BindPipeline(device, pipelines->GetBloomPipeline());

    pipelines->GetLightningDescriptor()->BindImage(0, EnigmaRHI::EImageType::TYPE_2D, srcTexture);
    pipelines->GetLightningDescriptor()->BindImage(1, EnigmaRHI::EImageType::TYPE_2D, bloomPass.BloomTexture());

    pipelines->GetBloomPipeline()->SendToGPU("exposure", camera.GetExposure());
    cmd.Draw(EnigmaRHI::EDrawMode::TRIANGLES, 3);
}

void RenderInstance::FXAAPass(EnigmaRHI::ICommandBuffer& cmd, unsigned int sourceImageID)
{
    pipelines->GetFXAAPipeline()->SendToGPU("texelStep", 1.f / resolution.x, 1.f / resolution.y);
    pipelines->GetLightningDescriptor()->BindImage(0, EnigmaRHI::EImageType::TYPE_2D, sourceImageID);
    cmd.Draw(EnigmaRHI::EDrawMode::TRIANGLES, 3);
}