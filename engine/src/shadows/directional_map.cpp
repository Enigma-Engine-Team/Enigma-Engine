#include "shadows/directional_map.h"
#include "debug/log.h"
#include "renderer/render_instance.h"
#include <iostream>
#include <random>

void DirectionalMap::Create(EnigmaRHI::IRenderInterface* rhi)
{
	CreateDepthMaps(rhi);
	CreateMatricesUBO(rhi);
}

void DirectionalMap::Destroy(EnigmaRHI::IRenderInterface* rhi)
{
	rhi->DeleteFramebuffer(shadowFBO);
	rhi->DeleteImage(depthMap);
}

void DirectionalMap::UpdateLightSpaceMatrix(const CameraProxy& camera, Light* light)
{
    std::vector<Math::Matrix4x4> lightMatrices = GetLightSpaceMatrices(camera, light);

    for (int i = 0; i < lightMatrices.size(); i++)
        matriceUBO->CopyData(&lightMatrices[i], sizeof(Math::Matrix4x4), i * sizeof(Math::Matrix4x4));

    matriceUBO->Unbind(EnigmaRHI::EBufferTarget::UNIFORM_BUFFER);
}

void DirectionalMap::CreateDepthMaps(EnigmaRHI::IRenderInterface* rhi)
{
	this->shadowResolution = 4096;

    shadowCascadeLevels = {
        shadowDistance / 50.0f,
        shadowDistance / 15.0f,
        shadowDistance / 5.0f,
        shadowDistance / 2.0f
    };

	shadowFBO = rhi->InstantiateFramebuffer();
	depthMap = rhi->InstantiateImage();

	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	EnigmaRHI::ImageSampler depthSampler
	{
		.imageType = EnigmaRHI::EImageType::TYPE_2D_ARRAY,
		.dimensionMode = EnigmaRHI::EDimensionMode::THREE_DIMENSION,
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
		.depth = static_cast<uint32_t>(shadowCascadeLevels.size() + 1),
		.internationalFormat = EnigmaRHI::EImageFormat::D_COMPONENT,
		.sampler = depthSampler,
		.format = EnigmaRHI::EImageFormat::D_COMPONENT,
		.data = nullptr,
	};

	depthMap->Create(depthData, false);

	EnigmaRHI::FramebufferInfo shadowFBOInfo
	{
		.width = static_cast<uint32_t>(shadowResolution),
		.height = static_cast<uint32_t>(shadowResolution),
		.depthAttachment = depthMap,
		.depthAttachmentMode = EnigmaRHI::EAttachment::DEPTH_ATTACHMENT
	};

	shadowFBO->Create(shadowFBOInfo);
}

void DirectionalMap::CreateMatricesUBO(EnigmaRHI::IRenderInterface* rhi)
{
	size_t bufferSize = sizeof(Math::Matrix4x4) * 16;
	matriceUBO = rhi->InstantiateBuffer();
	matriceUBO->Create(bufferSize, EnigmaRHI::EBufferTarget::UNIFORM_BUFFER, nullptr, EnigmaRHI::EBufferUsage::STATIC_DRAW);
	matriceUBO->CreateDescriptorBufferInfo();
	matriceUBO->bufferInfo.range = bufferSize;
}

std::vector<Math::Vector4D> DirectionalMap::GetFrustumCornersWorldSpace(const Math::Matrix4x4& projview)
{
    Math::Matrix4x4 inv = projview.Inverse();

    std::vector<Math::Vector4D> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const Math::Vector4D pt = inv * Math::Vector4D(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }

    return frustumCorners;
}


std::vector<Math::Vector4D> DirectionalMap::GetFrustumCornersWorldSpace(Math::Matrix4x4 proj, Math::Matrix4x4 view)
{
    return GetFrustumCornersWorldSpace(proj * view);
}

Math::Matrix4x4 DirectionalMap::GetLightSpaceMatrix(const Math::Vector3D& lightDir, const float nearPlane, const float farPlane, float FOV, Math::Matrix4x4 view, float aspectRatio, Math::Vector3D camPos)
{
    const auto proj = Math::Matrix4x4::Perspective(
        FOV, aspectRatio, nearPlane,
        farPlane);
    const auto corners = GetFrustumCornersWorldSpace(proj, view);

    Math::Vector3D center = Math::Vector3D::Zero;
    for (const auto& v : corners)
    {
        center += Math::Vector3D(v);
    }
    center /= corners.size();

    Math::Vector3D lightDirN = lightDir.Normalized();

    Math::Vector3D worldUp;
    if (std::abs(lightDirN.y) < 0.9f)
        worldUp = Math::Vector3D(0, 1, 0);
    else
        worldUp = Math::Vector3D(0, 0, 1);

    Math::Vector3D right = worldUp.CrossProduct(lightDirN).Normalized();
    Math::Vector3D up = lightDirN.CrossProduct(right).Normalized();

    Math::Matrix4x4 lightView = Math::Matrix4x4::LookAt(
        center - lightDirN, center, up);

    float minX = std::numeric_limits<float>::max(), maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max(), maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max(), maxZ = std::numeric_limits<float>::lowest();

    for (auto& c : corners)
    {
        Math::Vector4D trf = lightView * Math::Vector4D(c.x, c.y, c.z, 1.0f);
        minX = std::min(minX, trf.x); maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y); maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z); maxZ = std::max(maxZ, trf.z);
    }

    float zExtend = 50.0f;
    float xyExtend = 20.0f;

    minX -= xyExtend;
    maxX += xyExtend;
    minY -= xyExtend;
    maxY += xyExtend;
    minZ -= zExtend;
    maxZ += zExtend;
    minZ -= zExtend;
    maxZ += zExtend;

    Math::Matrix4x4 lightProj = Math::Matrix4x4::Orthographic(minX, maxX, minY, maxY, minZ, maxZ);
    return lightProj.NewMultiplyMatrix(lightView);
}

std::vector<Math::Matrix4x4> DirectionalMap::GetLightSpaceMatrices(const CameraProxy& camera, Light* light)
{
    std::vector<Math::Matrix4x4> ret;

    for (size_t i = 0; i < shadowCascadeLevels.size() + 1; ++i)
    {
        if (i == 0)
            ret.push_back(GetLightSpaceMatrix(light->GetDirection(),
                camera.GetZNear(), shadowCascadeLevels[i],
                camera.GetFOV(), camera.GetView(),
                camera.GetAspectRatio(), camera.GetPosition()));
        else if (i < shadowCascadeLevels.size())
            ret.push_back(GetLightSpaceMatrix(light->GetDirection(),
                shadowCascadeLevels[i - 1], shadowCascadeLevels[i],
                camera.GetFOV(), camera.GetView(),
                camera.GetAspectRatio(), camera.GetPosition()));
        else
            ret.push_back(GetLightSpaceMatrix(light->GetDirection(),
                shadowCascadeLevels[i - 1], camera.GetZFar(),
                camera.GetFOV(), camera.GetView(),
                camera.GetAspectRatio(), camera.GetPosition()));
    }
    return ret;
}
