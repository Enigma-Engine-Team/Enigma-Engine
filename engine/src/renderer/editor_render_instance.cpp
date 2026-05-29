#include "renderer/editor_render_instance.h"
#include "scenes/scene_manager.h"
#include "components/mesh_renderer.h"
#include "engine/engine.h"

void EditorRenderInstance::Setup(RenderContext* _ctx, RenderPipelines* _pipelines,
    RenderResources* _resources, EnigmaRHI::IRenderPass* _renderPass)
{
    ctx = _ctx;
    pipelines = _pipelines;
    resources = _resources;
    renderPass = _renderPass;

    gBuffer = new GBuffer();
    gBuffer->Create(ctx, (int)resolution.x, (int)resolution.y);

    EnigmaRHI::ImageSampler colorSampler
    {
        .imageType = EnigmaRHI::EImageType::TYPE_2D,
        .minFilter = EnigmaRHI::EFilteringMode::LINEAR,
        .magFilter = EnigmaRHI::EFilteringMode::LINEAR,
        .wrapU = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
        .wrapV = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
        .pixelType = EnigmaRHI::EDataType::FLOAT,
        .generateMipMaps = false,
    };

    EnigmaRHI::ImageSampler depthSampler
    {
        .imageType = EnigmaRHI::EImageType::TYPE_2D,
        .minFilter = EnigmaRHI::EFilteringMode::NEAREST,
        .magFilter = EnigmaRHI::EFilteringMode::NEAREST,
        .wrapU = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
        .wrapV = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
        .pixelType = EnigmaRHI::EDataType::UNSIGNED_INT_24_8,
        .generateMipMaps = false,
    };

    EnigmaRHI::ImageData colorData
    {
        .width = static_cast<uint32_t>(resolution.x),
        .height = static_cast<uint32_t>(resolution.y),
        .internalFormat = EnigmaRHI::EImageFormat::RGB16F,
        .sampler = colorSampler,
        .format = EnigmaRHI::EImageFormat::RGBA8,
        .data = nullptr
    };

    EnigmaRHI::ImageData depthData
    {
        .width = static_cast<uint32_t>(resolution.x),
        .height = static_cast<uint32_t>(resolution.y),
        .internalFormat = EnigmaRHI::EImageFormat::D32_SFLOAT_S8_UINT,
        .sampler = depthSampler,
        .format = EnigmaRHI::EImageFormat::D32_SFLOAT_S8_UINT,
        .data = nullptr
    };

    colorBuffer = ctx->rhi->InstantiateImage();
    depthBuffer = ctx->rhi->InstantiateImage();
    finalColorBuffer = ctx->rhi->InstantiateImage();
    brighnessBuffer = ctx->rhi->InstantiateImage();
    bloomColorBuffer = ctx->rhi->InstantiateImage();

    colorBuffer->Create(colorData);
    depthBuffer->Create(depthData);
    finalColorBuffer->Create(colorData);
    brighnessBuffer->Create(colorData);
    bloomColorBuffer->Create(colorData);

    lightingFBO = ctx->rhi->InstantiateFramebuffer();
    lightingFBO->Create({
        .width = static_cast<uint32_t>(resolution.x),
        .height = static_cast<uint32_t>(resolution.y),
        .colorAttachments = { colorBuffer, brighnessBuffer },
        .depthAttachment = depthBuffer
        });

    bloomFBO = ctx->rhi->InstantiateFramebuffer();
    bloomFBO->Create({
        .width = static_cast<uint32_t>(resolution.x),
        .height = static_cast<uint32_t>(resolution.y),
        .colorAttachments = { bloomColorBuffer },
        .depthAttachment = depthBuffer
        });

    finalFBO = ctx->rhi->InstantiateFramebuffer();
    finalFBO->Create({
        .width = static_cast<uint32_t>(resolution.x),
        .height = static_cast<uint32_t>(resolution.y),
        .colorAttachments = { finalColorBuffer },
        .depthAttachment = depthBuffer
        });

    bloomPass.Init(resolution.x, resolution.y, ctx->rhi);
    ssaoPass.Create(resolution.x, resolution.y, ctx->rhi);
}

void EditorRenderInstance::Render(Scene* scene, EnigmaRHI::IDevice* device, EnigmaRHI::IRenderPass* renderPass, EnigmaRHI::ICommandBuffer& cmd)
{
    camera.engineCam = scene->GetEngineCam();
    camera.gameCam = nullptr;
    camera.Update(resolution.x, resolution.y);

    resources->GetShadowSystem()->RenderShadows(scene, &LightManager::GetInstance(),
        cmd, renderPass, ctx->rhi, ctx->device,
        pipelines->GetMeshVertexMode(),
        pipelines->GetLightningDescriptor(), camera);

    RegisterGizmoCommands(scene);
    
    GeometryPass(scene, cmd);

    SSAOPass(scene, cmd);

    LightingPass(scene, cmd);

    RenderGrid(cmd);
    RenderGizmos(scene, device, cmd);

    lightingFBO->Unbind();

    BloomPass(lightingFBO->GetColorAttachmentID(0), renderPass, ctx->device, cmd);

    finalFBO->Bind();
    renderPass->ClearColor(0.f, 0.f, 0.f, 1.f);
    renderPass->ClearBuffer(EnigmaRHI::EMask::COLOR | EnigmaRHI::EMask::DEPTH);

    cmd.BindPipeline(ctx->device, pipelines->GetFXAAPipeline());
    FXAAPass(cmd, bloomFBO->GetColorAttachmentID(0));

    finalFBO->Unbind();
}

void EditorRenderInstance::RenderGrid(EnigmaRHI::ICommandBuffer& cmd)
{
    cmd.BindPipeline(ctx->device, resources->GetGrid()->GetPipeline());
    resources->GetGrid()->Render(cmd);
}

void EditorRenderInstance::RenderGizmos(Scene* scene, EnigmaRHI::IDevice* device, EnigmaRHI::ICommandBuffer& cmd)
{
    if (!resources->GetGizmoRenderer().IsGizmosActive()) return;

    cmd.BindPipeline(ctx->device, resources->GetGizmoRenderer().GetFrontPipeline());
    resources->GetGizmoRenderer().Render(Gizmos::GetCommands(), camera.GetVP(), device, cmd);
    cmd.BindPipeline(ctx->device, resources->GetGizmoRenderer().GetBackPipeline());
    resources->GetGizmoRenderer().Render(Gizmos::GetCommands(), camera.GetVP(), device, cmd, false);
}

void EditorRenderInstance::RegisterGizmoCommands(Scene* scene)
{
    if (!selectedGameObject)
        return;

    MeshRenderer* mr = selectedGameObject->GetComponent<MeshRenderer>();

    if (resources->GetGizmoRenderer().IsGizmosActive())
    {
        for (IComponent* c : selectedGameObject->GetComponents())
        {
            if (typeid(*c) != typeid(MeshRenderer))
                c->OnDraw(scene->GetEngineCam()->GetPosition());
        }
    }

    if (mr)
    {
        if (resources->GetGizmoRenderer().DisplayAABB())
            mr->OnDraw(scene->GetEngineCam()->GetPosition());
    }

}


unsigned int EditorRenderInstance::GetOutputTextureID() const
{
    return finalFBO->GetColorAttachmentID(0);
}

void EditorRenderInstance::Destroy()
{
    // delete gBuffer, FBOs, images...
}