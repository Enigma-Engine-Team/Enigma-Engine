#include "renderer/game_render_instance.h"
#include "renderer/widget_renderer.h"
#include "engine/engine.h"

void GameRenderInstance::Setup(RenderContext* _ctx, RenderPipelines* _pipelines,
    RenderResources* _resources, EnigmaRHI::IRenderPass* _renderPass)
{
    ctx = _ctx;
    pipelines = _pipelines;
    resources = _resources;
    renderPass = _renderPass;
    resolution = { 1920.f, 1080.f };

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
        .internationalFormat = EnigmaRHI::EImageFormat::RGB16F,
        .sampler = colorSampler,
        .format = EnigmaRHI::EImageFormat::RGBA8,
        .data = nullptr
    };

    EnigmaRHI::ImageData depthData
    {
        .width = static_cast<uint32_t>(resolution.x),
        .height = static_cast<uint32_t>(resolution.y),
        .internationalFormat = EnigmaRHI::EImageFormat::D32_SFLOAT_S8_UINT,
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

    finalFBO = ctx->rhi->InstantiateFramebuffer();
    finalFBO->Create({
        .width = static_cast<uint32_t>(resolution.x),
        .height = static_cast<uint32_t>(resolution.y),
        .colorAttachments = { finalColorBuffer },
        .depthAttachment = depthBuffer
        });

    bloomFBO = ctx->rhi->InstantiateFramebuffer();
    bloomFBO->Create({
        .width = static_cast<uint32_t>(resolution.x),
        .height = static_cast<uint32_t>(resolution.y),
        .colorAttachments = { bloomColorBuffer },
        .depthAttachment = depthBuffer
        });

    bloomPass.Init(resolution.x, resolution.y, ctx->rhi);
}

void GameRenderInstance::Render(Scene* scene, EnigmaRHI::IDevice* device, EnigmaRHI::IRenderPass* renderPass, EnigmaRHI::ICommandBuffer& cmd)
{
    if (scene->GetGameCam())
        camera.gameCam = scene->GetGameCam()->GetComponent<GameCamera>();
    else
        camera.gameCam = nullptr;

    if (!camera.gameCam)
    {
        cmd.BeginRenderPass(renderPass, ctx->swapChain, (int)resolution.x, (int)resolution.y);
        WidgetRenderer wr = WidgetRenderer::GetInstance();
        wr.GetTextRenderer()->Render("No game camera available !", resolution.x * .5f, resolution.y * .5f, 20.f, { 1.0f, 1.f, 1.f, 1.f }, wr.GetDefaultFontTexture(), cmd, ctx->device);
        cmd.EndRenderPass(ctx->swapChain);
        return;
    }

    camera.engineCam = nullptr;
    camera.Update(resolution.x, resolution.y);

    resources->GetShadowSystem()->RenderShadows(scene, &LightManager::GetInstance(),
        cmd, renderPass, ctx->rhi, ctx->device,
        pipelines->GetMeshVertexMode(),
        pipelines->GetLightningDescriptor(), camera);

    GeometryPass(scene, cmd);
    LightingPass(scene, cmd);

    lightingFBO->Unbind();

    BloomPass(lightingFBO->GetColorAttachmentID(0), renderPass, ctx->device, cmd);

    RenderWidgets(scene, cmd);

    bloomFBO->Unbind();

    if (Engine::IsInGameBuild())
    {
        cmd.BindPipeline(device, pipelines->GetBloomPipeline());
        renderPass->ClearBuffer(EnigmaRHI::EMask::COLOR | EnigmaRHI::EMask::DEPTH);
        cmd.BindPipeline(ctx->device, pipelines->GetFXAAPipeline());
        FXAAPass(cmd, bloomFBO->GetColorAttachmentID(0));
    }
    else
    {
        cmd.BindPipeline(ctx->device, pipelines->GetFXAAPipeline());
        cmd.BeginRenderPass(renderPass, ctx->swapChain, (int)resolution.x, (int)resolution.y);
        FXAAPass(cmd, bloomFBO->GetColorAttachmentID(0));
        cmd.EndRenderPass(ctx->swapChain);
    }
}


void GameRenderInstance::RenderWidgets(Scene* scene, EnigmaRHI::ICommandBuffer& cmd)
{
    WidgetRenderer::GetInstance().RenderWidgets(
        camera.GetVP(), camera.GetView(), cmd, ctx->GetDevice());
}

unsigned int GameRenderInstance::GetOutputTextureID() const
{
    return ctx->swapChain->renderFramebuffer->GetColorAttachmentID(0);
}

void GameRenderInstance::Destroy()
{
    // delete gBuffer, FBOs, images...
}