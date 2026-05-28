#include "renderer/bloom_pass.h"
#include <glad/glad.h>d

BloomRenderer::BloomRenderer() : mInit(false) {}
BloomRenderer::~BloomRenderer() {}

bool BloomRenderer::Init(unsigned int windowWidth, unsigned int windowHeight, EnigmaRHI::IRenderInterface* rhi)
{
	if (mInit) 
		return true;

	mSrcViewportSize = Math::Vector2D(windowWidth, windowHeight);
	mSrcViewportSizeFloat = Math::Vector2D((float)windowWidth, (float)windowHeight);

	// Framebuffer
	const unsigned int num_bloom_mips = 6; // TODO: Play around with this value
	bool status = mFBO.Init(windowWidth, windowHeight, num_bloom_mips);
	if (!status) {
		std::cerr << "Failed to initialize bloom FBO - cannot create bloom renderer!\n";
		return false;
	}

	// Shaders
	downsampleFrag = rhi->InstantiateShaderModule();
	downsampleVert = rhi->InstantiateShaderModule();

	downsampleFrag->Create("shaders/bloom_downsample.frag", EnigmaRHI::EShaderType::FRAGMENT);
	downsampleVert->Create("shaders/full_screen_quad.vert", EnigmaRHI::EShaderType::VERTEX);

	upsampleFrag = rhi->InstantiateShaderModule();
	upsampleVert = rhi->InstantiateShaderModule();

	upsampleFrag->Create("shaders/bloom_upsample.frag", EnigmaRHI::EShaderType::FRAGMENT);
	upsampleVert->Create("shaders/full_screen_quad.vert", EnigmaRHI::EShaderType::VERTEX);

	upsamplePipeline = rhi->InstantiatePipeline();
	downsamplePipeline = rhi->InstantiatePipeline();

	EnigmaRHI::GraphicsPipeline downsampleGP;
	downsampleGP.depthTestEnable = true;
	downsampleGP.depthWriteEnable = true;
	downsampleGP.frontFaceMode = EnigmaRHI::EFrontFaceMode::COUNTER_CLOCK_WISE;

	downsamplePipeline->Create(downsampleGP, downsampleVert, downsampleFrag);

	EnigmaRHI::GraphicsPipeline upsampleGP = downsampleGP;

	upsampleGP.blendEnable = true;
	upsampleGP.sourceFactor = EnigmaRHI::EBlendFactor::ONE;
	upsampleGP.destFactor = EnigmaRHI::EBlendFactor::ONE;
	upsampleGP.combinationMode = EnigmaRHI::EBlendOp::ADD;

	upsamplePipeline->Create(upsampleGP, upsampleVert, upsampleFrag);

	//mDownsampleShader = new Shader("6.new_downsample.vs", "6.new_downsample.fs");
	//mUpsampleShader = new Shader("6.new_upsample.vs", "6.new_upsample.fs");

	// Downsample
	//mDownsampleShader->use();
	//mDownsampleShader->setInt("srcTexture", 0);
	//glUseProgram(0);
	//
	//// Upsample
	//mUpsampleShader->use();
	//mUpsampleShader->setInt("srcTexture", 0);
	//glUseProgram(0);

	return true;
}

void BloomRenderer::Destroy()
{
	mFBO.Destroy();
	//delete mDownsampleShader;
	//delete mUpsampleShader;
}

void BloomRenderer::RenderDownsamples(unsigned int srcTexture, EnigmaRHI::IDevice* device, EnigmaRHI::ICommandBuffer& cmd)
{
	const std::vector<BloomMip>& mipChain = mFBO.MipChain();

	cmd.BindPipeline(device, downsamplePipeline);
	downsamplePipeline->SendToGPU("srcResolution", mSrcViewportSizeFloat.x, mSrcViewportSizeFloat.y);
	if (mKarisAverageOnDownsample)
	{
		downsamplePipeline->SendToGPU("mipLevel", 0);
	}

	// Bind srcTexture (HDR color buffer) as initial texture input
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcTexture);

	// Progressively downsample through the mip chain
	for (int i = 0; i < (int)mipChain.size(); i++)
	{
		const BloomMip& mip = mipChain[i];
		glViewport(0, 0, mip.size.x, mip.size.y);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, mip.texture, 0);

		// Render screen-filled quad of resolution of current mip
		cmd.Draw(EnigmaRHI::EDrawMode::TRIANGLES, 3);

		// Set current mip resolution as srcResolution for next iteration
		downsamplePipeline->SendToGPU("srcResolution", mip.size.x, mip.size.y);
		// Set current mip as texture input for next iteration
		glBindTexture(GL_TEXTURE_2D, mip.texture);
		// Disable Karis average for consequent downsamples
		if (i == 0)
		{ 
			downsamplePipeline->SendToGPU("mipLevel", 1);
		}
	}

	glUseProgram(0);
}

void BloomRenderer::RenderUpsamples(float filterRadius, EnigmaRHI::IDevice* device, EnigmaRHI::ICommandBuffer& cmd)
{
	const std::vector<BloomMip>& mipChain = mFBO.MipChain();

	cmd.BindPipeline(device, upsamplePipeline);
	upsamplePipeline->SendToGPU("filterRadius", filterRadius);

	// Enable additive blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	for (int i = (int)mipChain.size() - 1; i > 0; i--)
	{
		const BloomMip& mip = mipChain[i];
		const BloomMip& nextMip = mipChain[i - 1];

		// Bind viewport and texture from where to read
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mip.texture);

		// Set framebuffer render target (we write to this texture)
		glViewport(0, 0, nextMip.size.x, nextMip.size.y);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, nextMip.texture, 0);

		// Render screen-filled quad of resolution of current mip
		cmd.Draw(EnigmaRHI::EDrawMode::TRIANGLES, 3);
	}

	// Disable additive blending
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);

	glUseProgram(0);
}

void BloomRenderer::RenderBloomTexture(unsigned int srcTexture, float filterRadius, EnigmaRHI::IDevice* device, EnigmaRHI::ICommandBuffer& cmd)
{
	mFBO.BindForWriting();

	this->RenderDownsamples(srcTexture, device, cmd);
	this->RenderUpsamples(filterRadius, device, cmd);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Restore viewport
	glViewport(0, 0, mSrcViewportSize.x, mSrcViewportSize.y);
}

GLuint BloomRenderer::BloomTexture()
{
	return mFBO.MipChain()[0].texture;
}

GLuint BloomRenderer::BloomMip_i(int index)
{
	const std::vector<BloomMip>& mipChain = mFBO.MipChain();
	int size = (int)mipChain.size();
	return mipChain[(index > size - 1) ? size - 1 : (index < 0) ? 0 : index].texture;
}