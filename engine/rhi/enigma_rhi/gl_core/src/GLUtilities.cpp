#include "../include/GLUtilities.h"
#include <iostream>
#include <glad/glad.h>

unsigned int EnigmaRHI::GLUtilities::FormatToGLInternal(EnigmaRHI::EImageFormat format)
{
	switch (format)
	{
	case EImageFormat::R8:                  
		return GL_R8;
	case EImageFormat::RG8:                 
		return GL_RG8;
	case EImageFormat::RGBA:
		return GL_RGBA;
	case EImageFormat::RG16F:
		return GL_RG16F;
	case EImageFormat::RGB32F:
		return GL_RGB32F;
	case EImageFormat::RGB8:                
		return GL_RGB8;
	case EImageFormat::RGB16F:               
		return GL_RGB16F;
	case EImageFormat::RGBA8:               
		return GL_RGBA8;
	case EImageFormat::sRGB:
		return GL_SRGB8;
	case EImageFormat::sRGBA:
		return GL_SRGB8_ALPHA8;
	case EImageFormat::D24_UNORM_S8_UINT:   
		return GL_DEPTH24_STENCIL8;
	case EImageFormat::D32_SFLOAT_S8_UINT:  
		return GL_DEPTH32F_STENCIL8;
	case EnigmaRHI::EImageFormat::D_COMPONENT:
		return GL_DEPTH_COMPONENT32F;
	default:
		return 0;
	}
}

unsigned int EnigmaRHI::GLUtilities::FormatToGL(EnigmaRHI::EImageFormat format)
{
	switch (format)
	{
	case EnigmaRHI::EImageFormat::R8:
		return GL_RED;
	case EnigmaRHI::EImageFormat::RG8:
		return GL_RG;
	case EnigmaRHI::EImageFormat::RG16F:
		return GL_RG;
	case EnigmaRHI::EImageFormat::RGB8:
		return GL_RGB;
	case EnigmaRHI::EImageFormat::sRGB:
		return GL_RGB;
	case EnigmaRHI::EImageFormat::RGB16F:
		return GL_RGB;
	case EnigmaRHI::EImageFormat::RGB32F:
		return GL_RGB;
	case EnigmaRHI::EImageFormat::RGBA8:
		return GL_RGBA;
	case EnigmaRHI::EImageFormat::RGBA16F:
		return GL_RGBA;
	case EnigmaRHI::EImageFormat::sRGBA:
		return GL_RGBA;
	case EnigmaRHI::EImageFormat::D24_UNORM_S8_UINT:
		return GL_DEPTH_STENCIL;
	case EnigmaRHI::EImageFormat::D32_SFLOAT_S8_UINT:
		return GL_DEPTH_STENCIL;
	case EnigmaRHI::EImageFormat::D_COMPONENT:
		return GL_DEPTH_COMPONENT;

	default:
		return 0;
	}
}

int EnigmaRHI::GLUtilities::ImageTypeToGL(EImageType imageType)
{
	switch (imageType)
	{
	case EnigmaRHI::EImageType::TYPE_1D:
		return GL_TEXTURE_1D;

	case EnigmaRHI::EImageType::TYPE_2D:
		return GL_TEXTURE_2D;

	case EnigmaRHI::EImageType::TYPE_3D:
		return GL_TEXTURE_3D;

	case EnigmaRHI::EImageType::TYPE_2D_ARRAY:
		return GL_TEXTURE_2D_ARRAY;

	case EnigmaRHI::EImageType::TYPE_CUBE_MAP:
		return GL_TEXTURE_CUBE_MAP;

	default:
		return 0;
	}
}

int EnigmaRHI::GLUtilities::WrappingModeToGL(EWrappingMode wrappingMode)
{
	switch (wrappingMode)
	{
	case EnigmaRHI::EWrappingMode::REPEAT:
		return GL_REPEAT;

	case EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE:
		return GL_CLAMP_TO_EDGE;

	case EnigmaRHI::EWrappingMode::CLAMP_TO_BORDER:
		return GL_CLAMP_TO_BORDER;

	case EnigmaRHI::EWrappingMode::MIRRORED_REPEAT:
		return GL_MIRRORED_REPEAT;

	case EnigmaRHI::EWrappingMode::MIRROR_CLAMP_TO_EDGE:
		return GL_MIRROR_CLAMP_TO_EDGE;

	default:
		return 0;
	}
}

int EnigmaRHI::GLUtilities::FilteringModeToGL(EFilteringMode filteringMode)
{
	switch (filteringMode)
	{
	case EnigmaRHI::EFilteringMode::LINEAR:
		return GL_LINEAR;

	case EnigmaRHI::EFilteringMode::NEAREST:
		return GL_NEAREST;

	case EnigmaRHI::EFilteringMode::NEAREST_MIPMAP_LINEAR:
		return GL_NEAREST_MIPMAP_LINEAR;

	case EnigmaRHI::EFilteringMode::LINEAR_MIPMAP_NEAREST:
		return GL_LINEAR_MIPMAP_NEAREST;

	case EnigmaRHI::EFilteringMode::NEAREST_MIPMAP_NEAREST:
		return GL_NEAREST_MIPMAP_NEAREST;

	case EnigmaRHI::EFilteringMode::LINEAR_MIPMAP_LINEAR:
		return GL_LINEAR_MIPMAP_LINEAR;

	default:
		return 0;
	}
}

int EnigmaRHI::GLUtilities::DataTypeToGL(EDataType pixelType)
{
	switch (pixelType)
	{
	case EnigmaRHI::EDataType::FLOAT:
		return GL_FLOAT;

	case EnigmaRHI::EDataType::UNSIGNED_BYTE:
		return GL_UNSIGNED_BYTE;

	case EnigmaRHI::EDataType::DEPTH_COMPONENT:
		return GL_DEPTH_COMPONENT;

	case EnigmaRHI::EDataType::UNSIGNED_INT_24_8:
		return GL_UNSIGNED_INT_24_8;

	case EnigmaRHI::EDataType::FLOAT_32_INT_8:
		return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;

	default:
		return 0;
	}
}

unsigned int EnigmaRHI::GLUtilities::ShaderTypeToGL(EShaderType shaderType)
{
	switch (shaderType)
	{
	case EnigmaRHI::EShaderType::FRAGMENT:
		return GL_FRAGMENT_SHADER;

	case EnigmaRHI::EShaderType::VERTEX:
		return GL_VERTEX_SHADER;

	case EnigmaRHI::EShaderType::COMPUTE:
		return GL_COMPUTE_SHADER;

	case EnigmaRHI::EShaderType::GEOMETRY:
		return GL_GEOMETRY_SHADER;

	default:
		return 0;
	}
}

int EnigmaRHI::GLUtilities::BufferTargetToGL(EBufferTarget target)
{
	switch (target)
	{
	case EBufferTarget::ARRAY_BUFFER:
		return GL_ARRAY_BUFFER;
		break;
	case EBufferTarget::UNIFORM_BUFFER:
		return GL_UNIFORM_BUFFER;
		break;
	case EBufferTarget::ELEMENT_BUFFER:
		return GL_ELEMENT_ARRAY_BUFFER;
		break;
	case EBufferTarget::SHADER_STORAGE_BUFFER:
		return GL_SHADER_STORAGE_BUFFER;
		break;
	}
	return 0;
}

int EnigmaRHI::GLUtilities::CullModeToGL(ECullMode cullMode)
{
	switch (cullMode)
	{
	case ECullMode::BACK:
		return GL_BACK;

	case ECullMode::FRONT:
		return GL_FRONT;

	case ECullMode::FRONT_AND_BACK:
		return GL_FRONT_AND_BACK;

	default:
		return GL_BACK;
	}
}

int EnigmaRHI::GLUtilities::PolygonModeToGL(EPolygonMode polygonMode)
{
	switch (polygonMode)
	{
	case EPolygonMode::FILL:
		return GL_FILL;

	case EPolygonMode::LINE:
		return GL_LINE;

	case EPolygonMode::POINT:
		return GL_POINT;

	default:
		return GL_FILL;
	}
}

int EnigmaRHI::GLUtilities::FrontFaceModeToGL(EFrontFaceMode frontFaceMode)
{
	switch (frontFaceMode)
	{
	case EFrontFaceMode::CLOCK_WISE:
		return GL_CW;

	case EFrontFaceMode::COUNTER_CLOCK_WISE:
		return GL_CCW;

	default:
		return GL_CCW;
	}
}

int EnigmaRHI::GLUtilities::BlendFactorToGL(EBlendFactor blendFactor)
{
	switch (blendFactor)
	{
	case EBlendFactor::ZERO:
		return GL_ZERO;

	case EBlendFactor::ONE:
		return GL_ONE;

	case EBlendFactor::ONE_MINUS_SRC_ALPHA:
		return GL_ONE_MINUS_SRC_ALPHA;

	case EBlendFactor::SRC_ALPHA:
		return GL_SRC_ALPHA;

	default:
		return GL_ZERO;
	}
}

int EnigmaRHI::GLUtilities::BlendOpToGL(EBlendOp blendOp)
{
	switch (blendOp)
	{
	case EBlendOp::ADD:
		return GL_FUNC_ADD;

	case EBlendOp::MAX:
		return GL_MAX;

	case EBlendOp::MIN:
		return GL_MIN;

	case EBlendOp::SUBTRACT:
		return GL_FUNC_SUBTRACT;

	case EBlendOp::REVERSE_SUBTRACT:
		return GL_FUNC_REVERSE_SUBTRACT;

	default:
		return GL_FUNC_ADD;
	}
}

int EnigmaRHI::GLUtilities::DepthCompareOpToGL(EDepthCompareOp depthComapreOp)
{
	switch (depthComapreOp)
	{
	case EDepthCompareOp::ALWAYS:
		return GL_ALWAYS;

	case EDepthCompareOp::EQUAL:
		return GL_EQUAL;

	case EDepthCompareOp::GREATER:
		return GL_GREATER;

	case EDepthCompareOp::LEQUAL:
		return GL_LEQUAL;

	case EDepthCompareOp::LESS:
		return GL_LESS;

	case EDepthCompareOp::NEVER:
		return GL_NEVER;

	default:
		return GL_LESS;
	}
}

unsigned int EnigmaRHI::GLUtilities::BufferUsageToGL(EBufferUsage bufferUsage)
{
	switch (bufferUsage)
	{
	case EBufferUsage::STATIC_DRAW:
		return GL_STATIC_DRAW;
	case EBufferUsage::DYNAMIC_DRAW:
		return GL_DYNAMIC_DRAW;
	case EBufferUsage::STREAM_DRAW:
		return GL_STREAM_DRAW;
	default:
		return GL_STATIC_DRAW;
	}
}


int EnigmaRHI::GLUtilities::FramebufferTargetToGL(EFramebufferTarget target)
{
	switch (target)
	{
	case EFramebufferTarget::DRAW:
		return GL_DRAW_FRAMEBUFFER;
		break;
	case EFramebufferTarget::READ:
		return GL_READ_FRAMEBUFFER;
		break;
	case EFramebufferTarget::READ_DRAW:
		return GL_FRAMEBUFFER;
		break;
	}
	return 0;
}

unsigned int EnigmaRHI::GLUtilities::MaskToGL(EMask eMask)
{
	GLbitfield mask = 0;

	if (eMask & EMask::COLOR)
	{
		mask |= GL_COLOR_BUFFER_BIT;
	}

	if (eMask & EMask::DEPTH)
	{
		mask |= GL_DEPTH_BUFFER_BIT;
	}

	if (eMask & EMask::STENCIL)
	{
		mask |= GL_STENCIL_BUFFER_BIT;
	}

	return mask;
}

unsigned int EnigmaRHI::GLUtilities::DrawModeToGL(EDrawMode drawMode)
{
	switch (drawMode)
	{
	case EDrawMode::POINTS:
		return GL_POINTS;
	case EDrawMode::LINES:
		return GL_LINES;
	case EDrawMode::LINE_STRIP:
		return GL_LINE_STRIP;
	case EDrawMode::TRIANGLES:
		return GL_TRIANGLES;
	case EDrawMode::TRIANGLE_STRIP:
		return GL_TRIANGLE_STRIP;
	default:
		return GL_TRIANGLES;
	}
}

unsigned int EnigmaRHI::GLUtilities::PackingModeToGL(EPackingMode packingMode)
{
	switch (packingMode)
	{
	case EPackingMode::UNPACK_ALIGNMENT:
		return GL_UNPACK_ALIGNMENT;
	case EPackingMode::PACK_ALIGNMENT:
		return GL_PACK_ALIGNMENT;
	default:
		return GL_UNPACK_ALIGNMENT;
	}
}

unsigned int EnigmaRHI::GLUtilities::AttachmentToGL(EAttachment attachment)
{
	switch (attachment)
	{
	case EAttachment::DEPTH_ATTACHMENT:
		return GL_DEPTH_ATTACHMENT;
	case EAttachment::STENCIL_ATTACHMENT:
		return GL_STENCIL_ATTACHMENT;
	case EAttachment::DEPTH_STENCIL_ATTACHMENT:
		return GL_DEPTH_STENCIL_ATTACHMENT;
	default:
		return GL_DEPTH_ATTACHMENT;
	}
}
