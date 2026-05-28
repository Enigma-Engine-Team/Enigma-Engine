#pragma once

#include "IFormat.h"
#include <iostream>

namespace EnigmaRHI
{
	struct ImageSampler
	{
		EImageType imageType = EImageType::TYPE_2D;
		EDimensionMode dimensionMode = EDimensionMode::TWO_DIMENSION;
		EFilteringMode minFilter = EFilteringMode::NEAREST_MIPMAP_LINEAR;
		EFilteringMode magFilter = EFilteringMode::LINEAR;
		EWrappingMode wrapU = EWrappingMode::REPEAT;
		EWrappingMode wrapV = EWrappingMode::REPEAT;
		EWrappingMode wrapW = EWrappingMode::REPEAT;
		EDataType pixelType = EDataType::UNSIGNED_BYTE;
		bool generateMipMaps = true;
		float* borderColor = nullptr;
		bool returnOnCreate = false;
	};
}

