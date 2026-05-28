#pragma once

namespace EnigmaRHI
{
	enum class ECullMode
	{
		DISABLED,
		BACK,
		FRONT,
		FRONT_AND_BACK
	};

	enum class EPolygonMode
	{
		FILL,
		LINE,
		POINT
	};

	enum class EFrontFaceMode
	{
		CLOCK_WISE,
		COUNTER_CLOCK_WISE
	};

	enum class EBlendFactor
	{
		ZERO,
		ONE,
		ONE_MINUS_SRC_ALPHA,
		SRC_ALPHA
	};

	enum class EBlendOp
	{
		ADD,
		SUBTRACT,
		REVERSE_SUBTRACT,
		MIN,
		MAX,
	};

	enum class EDepthCompareOp
	{
		LESS,
		NEVER,
		EQUAL,
		LEQUAL,
		GREATER,
		ALWAYS
	};
}