#pragma once

#include <iostream>

namespace EnigmaRHI
{
	enum class EImageFormat
	{
		UNDEFINED,
		R8,
		RG8,
		RG16F,
		RGB8,
		RGB32F,
		RGBA32F,
		RGBA,
		RGB16F,
		RGBA8,
		RGBA16F,
		sRGB,
		sRGBA,
		D_COMPONENT,
		D24_UNORM_S8_UINT,
		D32_SFLOAT,
		D32_SFLOAT_S8_UINT,
	};

	enum class EImageType
	{
		UNDEFINED,
		TYPE_1D,
		TYPE_2D,
		TYPE_3D,
		TYPE_2D_ARRAY,
		TYPE_CUBE_MAP,
	};

	enum class EFilteringMode
	{
		LINEAR,
		NEAREST,
		NEAREST_MIPMAP_LINEAR,
		LINEAR_MIPMAP_NEAREST,
		NEAREST_MIPMAP_NEAREST,
		LINEAR_MIPMAP_LINEAR
	};

	enum class EWrappingMode
	{
		REPEAT,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER,
		MIRRORED_REPEAT,
		MIRROR_CLAMP_TO_EDGE
	};

	enum class EDataType
	{
		FLOAT,
		UNSIGNED_BYTE,
		DEPTH_COMPONENT,
		UNSIGNED_INT_24_8,
		FLOAT_32_INT_8
	};

	enum EShaderType
	{
		VERTEX,
		FRAGMENT,
		COMPUTE,
		GEOMETRY
	};

	enum EFramebufferTarget
	{
		DRAW,
		READ,
		READ_DRAW
	};

	enum class EBufferTarget
	{
		ARRAY_BUFFER,
		UNIFORM_BUFFER,
		ELEMENT_BUFFER,
		SHADER_STORAGE_BUFFER
	};

	enum EMask : unsigned int
	{
		COLOR = 0x01,
		DEPTH = 0x02,
		STENCIL = 0x04,
		ALL = COLOR | DEPTH | STENCIL
	};

	enum class EDrawMode
	{
		POINTS,
		LINES,
		LINE_STRIP,
		TRIANGLES,
		TRIANGLE_STRIP
	};

	enum class EPackingMode
	{
		PACK_ALIGNMENT,
		UNPACK_ALIGNMENT
	};

	enum class EDimensionMode
	{
		ONE_DIMENSION,
		TWO_DIMENSION,
		THREE_DIMENSION
	};

	enum class EBufferUsage
	{
		STATIC_DRAW,
		DYNAMIC_DRAW,
		STREAM_DRAW
	};

	enum class EAttachment
	{
		DEPTH_ATTACHMENT,
		STENCIL_ATTACHMENT,
		DEPTH_STENCIL_ATTACHMENT,
	};
}