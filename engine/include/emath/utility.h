#pragma once

namespace Math
{
	constexpr auto PI = 3.14159265358979323846f;
	constexpr auto tolerance = 1e-8f;
	constexpr auto posInfinity = 3.402823466e+38f;
	constexpr auto negInfinity = 1.175494351e-38f;

	bool IsEqualZero(float num);
	bool IsEqual(float a, float b);
	float Clamp(float, float, float);
	float Lerp(float a, float b, float t);
	inline float MyToRadians(float x) { return (x * PI) / 180.f; }
	inline float MyToDegrees(float x) { return x * 180.f / PI; }
}