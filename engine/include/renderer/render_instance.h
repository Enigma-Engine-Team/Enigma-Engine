#pragma once
#include "gbuffer.h"
#include "render_context.h"
#include "renderer/render_pipelines.h"
#include "renderer/render_resources.h"
#include "scenes/scene.h"
#include "bloom_pass.h"
#include "ssao_pass.h"

struct CameraProxy
{
    EngineCamera* engineCam = nullptr;
    GameCamera* gameCam = nullptr;

    EnigmaRHI::IBuffer* GetDataBuffer() const { return engineCam ? engineCam->GetCameraDataBuffer()     :gameCam->GetCameraDataBuffer();}
    Math::Vector3D GetPosition()        const { return engineCam ? engineCam->GetPosition()             :gameCam->GetPosition();}
    Math::Matrix4x4 GetVP()             const { return engineCam ? engineCam->GetVP()                   :gameCam->GetVP(); }
    Math::Matrix4x4 GetView()           const { return engineCam ? engineCam->GetView()                 :gameCam->GetView();}
    Math::Matrix4x4 GetProjection()     const { return engineCam ? engineCam->GetProjection()           :gameCam->GetProjection(); }
    Math::Frustum GetFrustum()          const { return engineCam ? engineCam->GetFrustum()              :gameCam->GetFrustum();}
    float GetZNear()                    const { return engineCam ? engineCam->GetZNear()                :gameCam->GetZNear(); }
    float GetZFar()                     const { return engineCam ? engineCam->GetZFar()                 :gameCam->GetZFar(); }
    float GetFOV()                      const { return engineCam ? engineCam->GetFOV()                  :gameCam->GetFOV(); }
    float GetAspectRatio()              const { return engineCam ? engineCam->GetAspectRatio()          :gameCam->GetAspectRatio(); }
    float GetExposure()                 const { return engineCam ? engineCam->GetExposure()             :gameCam->GetExposure(); }

    void Update(float width, float height)
    {
        if (engineCam) 
            engineCam->Update(width, height);
        else           
            gameCam->Update(width, height);
    }
};

class RenderInstance
{
public:
    virtual ~RenderInstance() = default;

    virtual void Setup(RenderContext* ctx, RenderPipelines* pipelines, RenderResources* resources, EnigmaRHI::IRenderPass* renderPass) = 0;
    virtual void Render(Scene* scene, EnigmaRHI::IDevice* device, EnigmaRHI::IRenderPass* renderPass, EnigmaRHI::ICommandBuffer& cmd) = 0;
    virtual void Destroy() = 0;

    virtual unsigned int GetOutputTextureID() const = 0;
    virtual void SetResolution(float w, float h) = 0;
    CameraProxy camera;

protected:

    void GeometryPass(Scene* scene, EnigmaRHI::ICommandBuffer& cmd);
    void LightingPass(Scene* scene, EnigmaRHI::ICommandBuffer& cmd);
	void SSAOPass(Scene* scene, EnigmaRHI::ICommandBuffer& cmd);
    void ForwardPass(EnigmaRHI::ICommandBuffer& cmd);
    void BloomPass(unsigned int srcTexture, EnigmaRHI::IRenderPass* renderPass, EnigmaRHI::IDevice* device, EnigmaRHI::ICommandBuffer& cmd);
    void FXAAPass(EnigmaRHI::ICommandBuffer& cmd, unsigned int sourceImageID);

    GBuffer* gBuffer = nullptr;
    EnigmaRHI::IFramebuffer* lightingFBO = nullptr;
    EnigmaRHI::IFramebuffer* finalFBO = nullptr;
    EnigmaRHI::IFramebuffer* bloomFBO = nullptr;
    EnigmaRHI::IImage* colorBuffer = nullptr;
    EnigmaRHI::IImage* bloomColorBuffer = nullptr;
    EnigmaRHI::IImage* brighnessBuffer = nullptr;
    EnigmaRHI::IImage* depthBuffer = nullptr;
    EnigmaRHI::IImage* finalColorBuffer = nullptr;

    Math::Vector2D resolution = { 1920.f, 1080.f };

    RenderContext* ctx = nullptr;
    RenderPipelines* pipelines = nullptr;
    RenderResources* resources = nullptr;
    EnigmaRHI::IRenderPass* renderPass = nullptr;

    std::vector<GameObject*>    transparentObjects;

    BloomRenderer bloomPass;
    SSAORenderer ssaoPass;

    void CreateBuffers();
    void CreateFBOs();
};