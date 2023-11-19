#pragma once

#include <cmath>
#include <cfloat>

constexpr float
PI{ 3.14159265358979323846f },
HALF_PI{ 1.57079632679489661923f },
QUARTER_PI{ 0.785398163397448309616f },
DOUBLE_PI{ 6.283185307179586476925f },
QUADRUPLE_PI{ 12.56637061435917295385f },
TO_DEGREES{ 180.0f / PI },
TO_RADIANS{ PI / 180.0f };

template<typename Type>
static inline Type Square(const Type& a)
{
	return a * a;
}

template<typename Type>
static inline Type Lerp(const Type& a, const Type& b, float smoothFactor)
{
	return a + smoothFactor * (b - a);
}

template<typename Type>
static inline Type Clamp(const Type& value, const Type& min, const Type& max)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

static inline bool AreEqual(float a, float b, float epsilon = FLT_EPSILON)
{
	return abs(a - b) < epsilon;
}

static inline float Saturate(float value)
{
	if (value < 0.0f) return 0.0f;
	if (value > 1.0f) return 1.0f;
	return value;
}