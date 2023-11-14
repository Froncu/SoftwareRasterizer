#pragma once

#include <cmath>
#include <cfloat>

template<typename Type>
inline Type Square(const Type& a)
{
	return a * a;
}

template<typename Type>
inline Type Lerp(const Type& a, const Type& b, float smoothFactor)
{
	return a + smoothFactor * (b - a);
}

inline bool AreEqual(float a, float b, float epsilon = FLT_EPSILON)
{
	return abs(a - b) < epsilon;
}

template<typename Type>
inline Type Clamp(const Type& value, const Type& min, const Type& max)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

inline float Saturate(float value)
{
	if (value < 0.0f) return 0.0f;
	if (value > 1.0f) return 1.0f;
	return value;
}