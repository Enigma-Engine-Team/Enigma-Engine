#include "emath/utility.h"

#include <cmath>
#include <iostream>
#include <algorithm>

bool Math::IsEqualZero(float num)
{
    return std::fabs(num) < tolerance;
}

bool Math::IsEqual(float a, float b)
{
    return std::fabs(a - b) < tolerance;
}

float Math::Clamp(float value, float min, float max)
{
    return std::clamp(value, min, max);
}

float Math::Lerp(float a, float b, float t)
{
    return a + t * (b - a);
}