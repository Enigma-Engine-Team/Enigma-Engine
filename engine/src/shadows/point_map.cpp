#include "shadows/point_map.h"

void PointMap::Create(EnigmaRHI::IRenderInterface* rhi)
{
    CreateDepthCubeMap(rhi);
    CreateMatricesUBO(rhi);
}

void PointMap::Destroy(EnigmaRHI::IRenderInterface* rhi)
{
    rhi->DeleteFramebuffer(shadowFBO);
    rhi->DeleteImage(depthMap);
    rhi->DeleteBuffer(matriceUBO);
}

void PointMap::CreateDepthCubeMap(EnigmaRHI::IRenderInterface* rhi)
{
    shadowResolution = 1024;

    EnigmaRHI::ImageSampler depthSampler
    {
        .imageType = EnigmaRHI::EImageType::TYPE_CUBE_MAP,
        .dimensionMode = EnigmaRHI::EDimensionMode::TWO_DIMENSION,
        .minFilter = EnigmaRHI::EFilteringMode::NEAREST,
        .magFilter = EnigmaRHI::EFilteringMode::NEAREST,
        .wrapU = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
        .wrapV = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
        .pixelType = EnigmaRHI::EDataType::FLOAT,
        .generateMipMaps = false,
        .returnOnCreate = false
    };

    EnigmaRHI::ImageData depthData
    {
        .width = static_cast<uint32_t>(shadowResolution),
        .height = static_cast<uint32_t>(shadowResolution),
        .depth = 6,
        .internalFormat = EnigmaRHI::EImageFormat::D_COMPONENT,
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

void PointMap::CreateMatricesUBO(EnigmaRHI::IRenderInterface* rhi)
{
    size_t bufferSize = sizeof(Math::Matrix4x4) * 6;
    matriceUBO = rhi->InstantiateBuffer();
    matriceUBO->Create(bufferSize, EnigmaRHI::EBufferTarget::UNIFORM_BUFFER, nullptr, EnigmaRHI::EBufferUsage::DYNAMIC_DRAW);
    matriceUBO->CreateDescriptorBufferInfo();
    matriceUBO->bufferInfo.range = bufferSize;
}

std::array<Math::Matrix4x4, 6> PointMap::BuildFaceMatrices(const Math::Vector3D& pos) const
{
    Math::Matrix4x4 proj = Math::Matrix4x4::Perspective(90.0f, 1.0f, nearPlane, farPlane);

    return {
        proj.NewMultiplyMatrix(Math::Matrix4x4::LookAt(pos, pos + Math::Vector3D(1, 0, 0), Math::Vector3D(0,-1, 0))),
        proj.NewMultiplyMatrix(Math::Matrix4x4::LookAt(pos, pos + Math::Vector3D(-1, 0, 0), Math::Vector3D(0,-1, 0))),
        proj.NewMultiplyMatrix(Math::Matrix4x4::LookAt(pos, pos + Math::Vector3D(0, 1, 0), Math::Vector3D(0, 0, 1))),
        proj.NewMultiplyMatrix(Math::Matrix4x4::LookAt(pos, pos + Math::Vector3D(0,-1, 0), Math::Vector3D(0, 0,-1))),
        proj.NewMultiplyMatrix(Math::Matrix4x4::LookAt(pos, pos + Math::Vector3D(0, 0, 1), Math::Vector3D(0,-1, 0))),
        proj.NewMultiplyMatrix(Math::Matrix4x4::LookAt(pos, pos + Math::Vector3D(0, 0,-1), Math::Vector3D(0,-1, 0))),
    };
}

void PointMap::UpdateLightSpaceMatrix(const CameraProxy& camera, Light* light)
{
    Math::Vector3D pos = light->GetPosition();
    auto faces = BuildFaceMatrices(pos);

    for (int i = 0; i < 6; ++i)
        matriceUBO->CopyData(&faces[i], sizeof(Math::Matrix4x4), i * sizeof(Math::Matrix4x4));

    matriceUBO->Unbind(EnigmaRHI::EBufferTarget::UNIFORM_BUFFER);

    lightSpaceMatrix = faces[0];
}