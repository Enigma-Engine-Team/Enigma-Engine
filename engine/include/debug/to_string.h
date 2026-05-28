#pragma once

#include <string>
#include "emath/emath.h"

inline std::string ToString(int value)
{
	return std::to_string(value);
}

inline std::string ToString(size_t value)
{
	return std::to_string(value);
}

inline std::string ToString(long long value)
{
	return std::to_string(value);
}

inline std::string ToString(float value)
{
	return std::to_string(value);
}

inline std::string ToString(double value)
{
	return std::to_string(value);
}

inline std::string ToString(unsigned int value)
{
	return std::to_string(value);
}

inline std::string ToString(const Math::Vector3D& value)
{
	return std::to_string(value.x) + ", " + std::to_string(value.y) + ", " + std::to_string(value.z);
}

inline std::string ToString(const char* value)
{
	return std::string(value);
}