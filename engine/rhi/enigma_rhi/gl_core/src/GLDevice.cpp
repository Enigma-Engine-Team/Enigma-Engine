#include "glad/glad.h"
#include "GLDevice.h"
#include "GLPipeline.h"
#include "GLUtilities.h"

namespace EnigmaRHI
{
	void GLDevice::Create()
	{
		gladLoadGL();
	}

	void GLDevice::ApplyPipelineCache(IPipeline* pipeline)
	{
		if (!pipelineCacheInitialized)
		{
			pipeline->API_GL().ApplyTotalGraphicsPipeline();
			pipelineCache = pipeline->API_GL().GetGraphicsPipeline();
			pipelineCacheInitialized = true;
			return;
		}

		GraphicsPipeline pipelineGP = pipeline->API_GL().GetGraphicsPipeline();

		if (pipelineCache.blendEnable != pipelineGP.blendEnable)
		{
			if (pipelineGP.blendEnable)
				glEnable(GL_BLEND);
			else
				glDisable(GL_BLEND);
		}

		if (pipelineGP.blendEnable)
		{
			if (pipelineCache.combinationMode != pipelineGP.combinationMode)
				glBlendEquation(GLUtilities::BlendOpToGL(pipelineGP.combinationMode));
			if (pipelineCache.destFactor != pipelineGP.destFactor || pipelineCache.sourceFactor != pipelineGP.sourceFactor)
				glBlendFunc(GLUtilities::BlendFactorToGL(pipelineGP.sourceFactor), GLUtilities::BlendFactorToGL(pipelineGP.destFactor));
		}
		
		if (pipelineCache.depthTestEnable != pipelineGP.depthTestEnable)
		{
			if (pipelineGP.depthTestEnable)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
		}
		
		if (pipelineCache.depthWriteEnable != pipelineGP.depthWriteEnable)
			glDepthMask(pipelineGP.depthWriteEnable);

		if (pipelineCache.compareMode != pipelineGP.compareMode)
			glDepthFunc(GLUtilities::DepthCompareOpToGL(pipelineGP.compareMode));
		
		if (pipelineCache.cullMode != pipelineGP.cullMode)
		{
			if (pipelineGP.cullMode != ECullMode::DISABLED)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);
		}

		if(pipelineCache.cullMode != pipelineGP.cullMode && pipelineGP.cullMode != ECullMode::DISABLED)
			glCullFace(GLUtilities::CullModeToGL(pipelineGP.cullMode));

		if (pipelineCache.polygonMode != pipelineGP.polygonMode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GLUtilities::PolygonModeToGL(pipelineGP.polygonMode));
		}

		if (pipelineCache.frontFaceMode != pipelineGP.frontFaceMode)
			glFrontFace(GLUtilities::FrontFaceModeToGL(pipelineGP.frontFaceMode));

		if(pipelineCache.lineWidth != pipelineGP.lineWidth)
			glLineWidth(pipelineGP.lineWidth);

		pipelineCache = pipelineGP;
	}
}