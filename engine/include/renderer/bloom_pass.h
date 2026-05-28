#pragma once

#include "bloom_fbo.h"
#include "IRenderInterface.h"

class BloomRenderer
{
public:
	BloomRenderer();
	~BloomRenderer();
	bool Init(unsigned int windowWidth, unsigned int windowHeight, EnigmaRHI::IRenderInterface* rhi);
	void Destroy();
	void RenderBloomTexture(unsigned int srcTexture, float filterRadius, EnigmaRHI::IDevice* device, EnigmaRHI::ICommandBuffer& cmd);
	unsigned int BloomTexture();
	unsigned int BloomMip_i(int index);

private:
	void RenderDownsamples(unsigned int srcTexture, EnigmaRHI::IDevice* device, EnigmaRHI::ICommandBuffer& cmd);
	void RenderUpsamples(float filterRadius, EnigmaRHI::IDevice* device, EnigmaRHI::ICommandBuffer& cmd);

	bool mInit;
	BloomFBO mFBO;
	Math::Vector2D mSrcViewportSize;
	Math::Vector2D mSrcViewportSizeFloat;

	EnigmaRHI::IShaderModule* downsampleVert;
	EnigmaRHI::IShaderModule* downsampleFrag;
	EnigmaRHI::IShaderModule* upsampleVert;
	EnigmaRHI::IShaderModule* upsampleFrag;

	EnigmaRHI::IPipeline* downsamplePipeline;
	EnigmaRHI::IPipeline* upsamplePipeline;

	bool mKarisAverageOnDownsample = true;
};
