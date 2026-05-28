#pragma once

#include "IPipelineState.h"

namespace EnigmaRHI
{
	struct GraphicsPipeline
	{
		ECullMode cullMode = ECullMode::BACK;
		EFrontFaceMode frontFaceMode = EFrontFaceMode::CLOCK_WISE;

		bool blendEnable = false;

		EBlendFactor sourceFactor = EBlendFactor::ONE;
		EBlendFactor destFactor = EBlendFactor::ZERO;
		EBlendOp combinationMode = EBlendOp::ADD;

		bool depthTestEnable = false;

		EDepthCompareOp compareMode = EDepthCompareOp::LESS;

		EPolygonMode polygonMode = EPolygonMode::FILL;
		float lineWidth = 1.f;

		bool depthWriteEnable = false;
	};
}