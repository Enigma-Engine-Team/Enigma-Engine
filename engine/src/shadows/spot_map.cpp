#include "shadows/spot_map.h"

void SpotMap::Create(EnigmaRHI::IRenderInterface* rhi)
{
    CreateDepthMap(rhi);
    CreateMatricesUBO(rhi);
}

void SpotMap::Destroy(EnigmaRHI::IRenderInterface* rhi)
{
    rhi->DeleteFramebuffer(shadowFBO);
    rhi->DeleteImage(depthMap);
    rhi->DeleteBuffer(matriceUBO);
}

void SpotMap::CreateDepthMap(EnigmaRHI::IRenderInterface* rhi)
{
    shadowResolution = 2048;

    float borderColor[] = { 1.f, 1.f, 1.f, 1.f };

    EnigmaRHI::ImageSampler depthSampler
    {
        .imageType = EnigmaRHI::EImageType::TYPE_2D,
        .dimensionMode = EnigmaRHI::EDimensionMode::TWO_DIMENSION,
        .minFilter = EnigmaRHI::EFilteringMode::NEAREST,
        .magFilter = EnigmaRHI::EFilteringMode::NEAREST,
        .wrapU = EnigmaRHI::EWrappingMode::CLAMP_TO_BORDER,
        .wrapV = EnigmaRHI::EWrappingMode::CLAMP_TO_BORDER,
        .pixelType = EnigmaRHI::EDataType::FLOAT,
        .generateMipMaps = false,
        .borderColor = borderColor,
        .returnOnCreate = false
    };

    EnigmaRHI::ImageData depthData
    {
        .width = static_cast<uint32_t>(shadowResolution),
        .height = static_cast<uint32_t>(shadowResolution),
        .depth = 1,
        .internationalFormat = EnigmaRHI::EImageFormat::D_COMPONENT,
        .sampler = depthSampler,
        .format = EnigmaRHI::EImageFormat::D_COMPONENT,
        .data = nullptr,
    };

    depthMap = rhi->InstantiateImage();
    depthMap->Create(depthData, false);

    EnigmaRHI::FramebufferInfo fboInfo
    {
        .width = static_cast<uint32_t>(shadowResolution),
        .height = static_cast<uint32_t>(shadowResolution),
        .depthAttachment = depthMap,
        .depthAttachmentMode = EnigmaRHI::EAttachment::DEPTH_ATTACHMENT
    };

    shadowFBO = rhi->InstantiateFramebuffer();
    shadowFBO->Create(fboInfo);
}

void SpotMap::CreateMatricesUBO(EnigmaRHI::IRenderInterface* rhi)
{
    // Une seule matrice pour un spot light
    size_t bufferSize = sizeof(Math::Matrix4x4);
    matriceUBO = rhi->InstantiateBuffer();
    matriceUBO->Create(bufferSize, EnigmaRHI::EBufferTarget::UNIFORM_BUFFER,
        nullptr, EnigmaRHI::EBufferUsage::DYNAMIC_DRAW);
    matriceUBO->CreateDescriptorBufferInfo();
    matriceUBO->bufferInfo.range = bufferSize;
}

void SpotMap::UpdateLightSpaceMatrix(const CameraProxy& camera, Light* light)
{
    Math::Matrix4x4 proj = Math::Matrix4x4::Perspective(light->GetOuterCutOff(), 1.0f, nearPlane, farPlane);

    Math::Vector3D pos = light->GetPosition();
    Math::Vector3D dir = light->GetDirection().Normalized();
    Math::Vector3D up = (std::abs(dir.y) < 0.999f) ? Math::Vector3D::Up : Math::Vector3D::Right;

    Math::Matrix4x4 view = Math::Matrix4x4::LookAt(pos, pos + dir, up);
    lightSpaceMatrix = proj.NewMultiplyMatrix(view);

    matriceUBO->CopyData(&lightSpaceMatrix, sizeof(Math::Matrix4x4), 0);
    matriceUBO->Unbind(EnigmaRHI::EBufferTarget::UNIFORM_BUFFER);
}