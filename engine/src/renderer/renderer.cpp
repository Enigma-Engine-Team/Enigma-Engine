#include "renderer/renderer.h"
#include "scenes/scene_graph.h"
#include "engine/engine.h"

void Renderer::Create(RenderContext* context)
{
    ctx = context;

    renderPass = ctx->rhi->InstantiateRenderPass();
    renderPass->Create();

    renderPipeline.Create(ctx->rhi);
    renderResources.Create(ctx->rhi);
}

void Renderer::SetupPipelines()
{
    if (!Engine::IsInGameBuild())
    {
        editorInstance = new EditorRenderInstance();
        editorInstance->Setup(ctx, &renderPipeline, &renderResources, renderPass);
    }

    gameInstance = new GameRenderInstance();
    gameInstance->Setup(ctx, &renderPipeline, &renderResources, renderPass);
}

void Renderer::DrawFrame(Scene* scene)
{
    // Environment
    EnvironmentSystem* enviro = renderResources.GetEnvironmentSystem();
    enviro->SetCurrentSkybox(scene->GetSkyboxPath(), ctx->rhi);
    if (enviro->NeedToCompute())
        enviro->ComputeEnvironment(ctx, ctx->commandPool->GetCommandBuffer(),
            renderPipeline.GetGeometryDescriptor(),
            renderPipeline.GetLightningDescriptor(), renderPass);

    // Camera buffers
    if (!Engine::IsInGameBuild())
    {
        if (!scene->GetEngineCam()->GetCameraDataBuffer())
            scene->GetEngineCam()->CreateCameraDataBuffer(ctx->rhi);
    }
    
    for (int i = 0; i < scene->GetGameObjects().size(); i++)
    {
        if (GameCamera* gC = scene->GetGameObject(i)->GetComponent<GameCamera>())
        {
            if (!gC->GetCameraDataBuffer())
                gC->CreateCameraDataBuffer(ctx->rhi);

            if (!scene->GetGameCam())
                scene->SetGameCam(scene->GetGameObject(i));
        }
    }

    SceneGraph::GetInstance().Update(scene);

    EnigmaRHI::ICommandBuffer& cmd = ctx->commandPool->GetCommandBuffer();

    cmd.Begin();

    gameInstance->Render(scene, ctx->device, renderPass, cmd);

    if (!Engine::IsInGameBuild())
    {
        editorInstance->Render(scene, ctx->device, renderPass, cmd);
        Gizmos::Clear();
    }

    cmd.End();
}

void Renderer::Destroy()
{
    if (editorInstance)
    {
        editorInstance->Destroy();
        delete editorInstance;
    }
    
    gameInstance->Destroy();
    delete gameInstance;

    ctx->rhi->DeleteRenderPass(renderPass);
    renderPipeline.Destroy(ctx->rhi);
    renderResources.Destroy();
}