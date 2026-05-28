#pragma once
#include "render_context.h"
#include "renderer/render_pipelines.h"
#include "renderer/render_resources.h"
#include "renderer/editor_render_instance.h"
#include "renderer/game_render_instance.h"

class Renderer
{
public:
    Renderer() = default;
    void Create(RenderContext* context);
    void SetupPipelines();
    void DrawFrame(Scene* scene);
    void Destroy();

    unsigned int GetEditorTexture() const { return editorInstance->GetOutputTextureID(); }
    unsigned int GetGameTexture()   const { return gameInstance->GetOutputTextureID(); }

    void SetSelectedGameObject(GameObject* go) { selectedGameObject = go; editorInstance->SetSelectedGameObject(go); }
    void SetViewportSize(float w, float h) { editorInstance->SetResolution(w, h); }
    GizmoRenderer& GetGizmoRenderer() { return renderResources.GetGizmoRenderer(); }
    GameObject* GetSelectedGameObject() const { return selectedGameObject; };

private:
    RenderContext* ctx = nullptr;
    RenderPipelines  renderPipeline;
    RenderResources  renderResources;

    EnigmaRHI::IRenderPass* renderPass = nullptr;

    EditorRenderInstance* editorInstance = nullptr;
    GameRenderInstance* gameInstance = nullptr;

    GameObject* selectedGameObject = nullptr;
};