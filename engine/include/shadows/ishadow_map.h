#pragma once
#include "IRenderInterface.h"
#include "emath/emath.h"
#include "components/light.h"

struct CameraProxy;

enum class EShadowMapType { Directional, Spot, Point };

class IShadowMap
{
public:
    virtual void Create(EnigmaRHI::IRenderInterface* rhi) = 0;
    virtual void Destroy(EnigmaRHI::IRenderInterface* rhi) = 0;
    virtual void UpdateLightSpaceMatrix(const CameraProxy& camera, Light* light) = 0;
    virtual EShadowMapType GetType() const = 0;

    EnigmaRHI::IImage* GetDepth()           const { return depthMap; }
    Math::Matrix4x4        GetLightSpaceMatrix() const { return lightSpaceMatrix; }
    int                    GetShadowResolution() const { return shadowResolution; }
    EnigmaRHI::IBuffer* GetMatricesUBO()      const { return matriceUBO; }

    void BindShadowFBO() { shadowFBO->Bind(); }
    void UnbindShadowFBO() { shadowFBO->Unbind(); }

protected:
    EnigmaRHI::IFramebuffer* shadowFBO = nullptr;
    EnigmaRHI::IImage* depthMap = nullptr;
    EnigmaRHI::IBuffer* matriceUBO = nullptr;
    Math::Matrix4x4 lightSpaceMatrix;
    int shadowResolution = 2048;
};