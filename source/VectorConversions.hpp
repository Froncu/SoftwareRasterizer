#pragma once

#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

inline Vector2 GetVector2(const Vector3& vector)
{
	return Vector2
	(
		vector.x,
		vector.y
	);
}

inline Vector2 GetVector2(const Vector4& vector)
{
	return Vector2
	(
		vector.x,
		vector.y
	);
}

inline Vector3 GetVector3(const Vector2& vector)
{
	return Vector3
	(
		vector.x,
		vector.y,
		0.0f
	);
}

inline Vector3 GetVector3(const Vector4& vector)
{
	return Vector3
	(
		vector.x,
		vector.y,
		vector.z
	);
}

inline Vector4 GetVector4(const Vector2& vector)
{
	return Vector4
	(
		vector.x,
		vector.y,
		0.0f,
		0.0f
	);
}

inline Vector4 GetPoint4(const Vector2& vector)
{
	return Vector4
	(
		vector.x,
		vector.y,
		0.0f,
		1.0f
	);
}

inline Vector4 GetVector4(const Vector3& vector)
{
	return Vector4
	(
		vector.x,
		vector.y,
		vector.z,
		0.0f
	);
}

inline Vector4 GetPoint4(const Vector3& vector)
{
	return Vector4
	(
		vector.x,
		vector.y,
		vector.z,
		1.0f
	);
}