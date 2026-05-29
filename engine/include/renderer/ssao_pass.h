#pragma once

#include "IRenderInterface.h"
#include "emath/emath.h"
#include <array>

class GBuffer;

struct SSAOHeader
{
	Math::Vector4D samples[64];

	Math::Matrix4x4 projection;
	Math::Matrix4x4 view;

	float radius;
	float bias;
	int kernelSize;
	float width;

	float height;
	float padding[3];
};

struct SSAOEntry
{
	EnigmaRHI::IFramebuffer* ssaoFBO;
	EnigmaRHI::IImage* ssaoColorBuffer;
	EnigmaRHI::IPipeline* ssaoPipeline;
	EnigmaRHI::IDescriptor* ssaoDescriptor;
};

class SSAORenderer
{
public:

	SSAORenderer() = default;
	void Create(float width, float height, EnigmaRHI::IRenderInterface* rhi);
	void Destroy(EnigmaRHI::IRenderInterface* rhi);

	SSAOEntry GetSSAOEntry() const { return SSAOEntry{ ssaoFBO, ssaoColorBuffer, ssaoPipeline, ssaoDescriptor }; }
	SSAOEntry GetSSAOBlurEntry() const { return SSAOEntry{ ssaoBlurFBO, ssaoBlurColorBuffer, ssaoBlurPipeline, ssaoDescriptor }; }

	void BindForLighning(GBuffer* currentGBuffer, const Math::Vector2D& resolution, const Math::Matrix4x4& projectionMatrix, const Math::Matrix4x4& viewMatrix);
	void Render(EnigmaRHI::IDevice* device, EnigmaRHI::IRenderPass* renderPass, EnigmaRHI::ICommandBuffer& cmd);

private:

	void CreateDescriptor(EnigmaRHI::IRenderInterface* rhi);
	void CreateHeaderBuffer(EnigmaRHI::IRenderInterface* rhi);
	void CreatePipelines(EnigmaRHI::IRenderInterface* rhi);
	void CreateFBOs(float width, float height, EnigmaRHI::IRenderInterface* rhi);
	void CreateNoiseTexture(std::array<Math::Vector3D, 16>  noiseTextureData, EnigmaRHI::IRenderInterface* rhi);
	std::array<Math::Vector3D, 16> ComputeNoise(EnigmaRHI::IRenderInterface* rhi);

	EnigmaRHI::IFramebuffer* ssaoFBO = nullptr;
	EnigmaRHI::IImage* ssaoColorBuffer = nullptr;

	EnigmaRHI::IFramebuffer* ssaoBlurFBO = nullptr;
	EnigmaRHI::IImage* ssaoBlurColorBuffer = nullptr;

	EnigmaRHI::IImage* noiseTexture = nullptr;

	EnigmaRHI::IPipeline* ssaoPipeline = nullptr;
	EnigmaRHI::IShaderModule* ssaoFrag = nullptr;
	EnigmaRHI::IShaderModule* ssaoVert = nullptr;

	EnigmaRHI::IPipeline* ssaoBlurPipeline = nullptr;
	EnigmaRHI::IShaderModule* ssaoBlurFrag = nullptr;
	EnigmaRHI::IShaderModule* ssaoBlurVert = nullptr;

	EnigmaRHI::IDescriptor* ssaoDescriptor = nullptr;

	EnigmaRHI::IBuffer* ssaoHeaderBuffer = nullptr;

	std::vector<Math::Vector4D> ssaoKernel;

	int kernelSize = 32;
	float radius = 0.5f;
	float bias = 0.025f;
};