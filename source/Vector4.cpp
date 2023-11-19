#include "Vector4.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Mathematics.hpp"

#pragma region Operators
Vector4 Vector4::operator*(float scalar) const
{
	return Vector4
	(
		x * scalar,
		y * scalar,
		z * scalar,
		w * scalar
	);
}

Vector4 Vector4::operator/(float scalar) const
{
	return Vector4
	(
		x / scalar,
		y / scalar,
		z / scalar,
		w / scalar
	);
}

Vector4 Vector4::operator+(const Vector4& vector) const
{
	return Vector4
	(
		x + vector.x,
		y + vector.y,
		z + vector.z,
		w + vector.w
	);
}

Vector4 Vector4::operator-(const Vector4& vector) const
{
	return Vector4
	(
		x - vector.x,
		y - vector.y,
		z - vector.z,
		w - vector.w
	);
}

Vector4& Vector4::operator*=(float scalar)
{
	return *this = *this * scalar;
}

Vector4& Vector4::operator/=(float scalar)
{
	return *this = *this / scalar;
}

Vector4& Vector4::operator+=(const Vector4& vector)
{
	return *this = *this + vector;
}

Vector4& Vector4::operator-=(const Vector4& vector)
{
	return *this = *this - vector;
}

bool Vector4::operator==(const Vector4& vector) const
{
	return
		AreEqual(x, vector.x) &&
		AreEqual(y, vector.y) &&
		AreEqual(z, vector.z) &&
		AreEqual(w, vector.w);
}

float& Vector4::operator[](int index)
{
	switch (index)
	{
	default:
		return x;

	case 1:
		return y;

	case 2:
		return z;

	case 3:
		return w;
	}
}

float Vector4::operator[](int index) const
{
	switch (index)
	{
	default:
		return x;

	case 1:
		return y;

	case 2:
		return z;

	case 3:
		return w;
	}
}

Vector4 operator*(float scalar, const Vector4& vector)
{
	return vector * scalar;
}
#pragma endregion



#pragma region Public Methods
float Vector4::Dot(const Vector4& vector1, const Vector4& vector2)
{
	return vector1.x * vector2.x + vector1.y * vector2.y + vector1.z * vector2.z + vector1.w * vector2.w;
}

float Vector4::GetSquareMagnitude() const
{
	return x * x + y * y + z * z + w * w;
}

float Vector4::GetMagnitude() const
{
	return sqrtf(GetSquareMagnitude());
}

Vector4 Vector4::GetNormalized() const
{
	const float magnitude{ GetMagnitude() };
	return Vector4
	(
		x / magnitude,
		y / magnitude,
		z / magnitude,
		w / magnitude
	);
}

const Vector4& Vector4::Normalize()
{
	*this = GetNormalized();
	return *this;
}

Vector2 Vector4::GetVector2() const
{
	return Vector2
	(
		x,
		y
	);
}

Vector3 Vector4::GetVector3() const
{
	return Vector3
	(
		x,
		y,
		z
	);
}
#pragma endregion