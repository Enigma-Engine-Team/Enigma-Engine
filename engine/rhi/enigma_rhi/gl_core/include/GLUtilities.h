#pragma once

#include "IFormat.h"
#include "IGraphicsPipeline.h"

namespace EnigmaRHI
{
	struct GLUtilities
	{
		//Image
		static unsigned int FormatToGLInternal(EImageFormat format);
		static unsigned int FormatToGL(EImageFormat format);
		static unsigned int ShaderTypeToGL(EShaderType shaderType);
		static int ImageTypeToGL(EImageType imageType);
		static int WrappingModeToGL(EWrappingMode wrappingMode);
		static int FilteringModeToGL(EFilteringMode filteringMode);
		static int DataTypeToGL(EDataType pixelType);

		// Buffer
		static int BufferTargetToGL(EBufferTarget target);
		static unsigned int BufferUsageToGL(EBufferUsage bufferUsage);
		static int FramebufferTargetToGL(EFramebufferTarget target);

		// Graphics Pipeline
		static int CullModeToGL(ECullMode cullMode);
		static int PolygonModeToGL(EPolygonMode polygonMode);
		static int FrontFaceModeToGL(EFrontFaceMode frontFaceMode);
		static int BlendFactorToGL(EBlendFactor blendFactor);
		static int BlendOpToGL(EBlendOp blendOp);
		static int DepthCompareOpToGL(EDepthCompareOp depthComapreOp);

		static unsigned int MaskToGL(EMask mask);
		static unsigned int DrawModeToGL(EDrawMode drawMode);

		static unsigned int PackingModeToGL(EPackingMode packingMode);

		static unsigned int AttachmentToGL(EAttachment attachment);
	};
}
