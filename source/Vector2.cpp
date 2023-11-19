#include "Vector2.h"

#include "Vector3.h"
#include "Vector4.h"
#include "Mathematics.hpp"

#pragma region Operators
Vector2 Vector2::operator*(float scalar) const
{
	return Vector2
	(
		x * scalar,
		y * scalar
	);
}

Vector2 Vector2::operator/(float scalar) const
{
	return Vector2
	(
		x / scalar,
		y / scalar
	);
}

Vector2 Vector2::operator+(const Vector2& vector) const
{
	return Vector2
	(
		x + vector.x,
		y + vector.y
	);
}

Vector2 Vector2::operator-(const Vector2& vector) const
{
	return Vector2
	(
		x - vector.x,
		y - vector.y
	);
}

Vector2 Vector2::operator-() const
{
	return Vector2
	(
		-x,
		-y
	);
}

Vector2& Vector2::operator*=(float scalar)
{
	return *this = *this * scalar;
}

Vector2& Vector2::operator/=(float scalar)
{
	return *this = *this / scalar;
}

Vector2& Vector2::operator+=(const Vector2& vector)
{
	return *this = *this + vector;
}

Vector2& Vector2::operator-=(const Vector2& vector)
{
	return *this = *this - vector;
}

bool Vector2::operator==(const Vector2& vector) const
{
	return
		AreEqual(x, vector.x) &&
		AreEqual(y, vector.y);
}

float& Vector2::operator[](int index)
{
	switch (index)
	{
	default:
		return x;

	case 1:
		return y;
	}
}

float Vector2::operator[](int index) const
{
	switch (index)
	{
	default:
		return x;

	case 1:
		return y;
	}
}

Vector2 operator*(float scalar, const Vector2& vector)
{
	return vector * scalar;
}
#pragma endregion



#pragma region Public Methods
float Vector2::Dot(const Vector2& vector1, const Vector2& vector2)
{
	return vector1.x * vector2.x + vector1.y * vector2.y;
}

float Vector2::Cross(const Vector2& vector1, const Vector2& vector2)
{
	return vector1.x * vector2.y - vector1.y * vector2.x;
}

float Vector2::GetSquareMagnitude() const
{
	return x * x + y * y;
}

float Vector2::GetMagnitude() const
{
	return sqrtf(GetSquareMagnitude());
}

Vector2 Vector2::GetNormalized() const
{
	const float magnitude{ GetMagnitude() };
	return Vector2
	(
		x / magnitude,
		y / magnitude
	);
}

const Vector2& Vector2::Normalize()
{
	*this = GetNormalized();
	return *this;
}

Vector3 Vector2::GetVector3() const
{
	return Vector3
	(
		x,
		y,
		0
	);
}

Vector4 Vector2::GetVector4() const
{
	return Vector4
	(
		x,
		y,
		0,
		0
	);
}

Vector4 Vector2::GetPoint4() const
{
	return Vector4
	(
		x,
		y,
		0,
		1
	);
}
#pragma endregion