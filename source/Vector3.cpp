#include "Vector3.h"

#include "Vector2.h"
#include "Vector4.h"
#include "Mathematics.hpp"

#pragma region Operators
Vector3 Vector3::operator*(float scalar) const
{
	return Vector3
	(
		x * scalar,
		y * scalar,
		z * scalar
	);
}

Vector3 Vector3::operator/(float scalar) const
{
	return Vector3
	(
		x / scalar,
		y / scalar,
		z / scalar
	);
}

Vector3 Vector3::operator+(const Vector3& vector) const
{
	return Vector3
	(
		x + vector.x,
		y + vector.y,
		z + vector.z
	);
}

Vector3 Vector3::operator-(const Vector3& vector) const
{
	return Vector3
	(
		x - vector.x,
		y - vector.y,
		z - vector.z
	);
}

Vector3 Vector3::operator-() const
{
	return Vector3
	(
		-x,
		-y,
		-z
	);
}

Vector3& Vector3::operator*=(float scalar)
{
	return *this = *this * scalar;
}

Vector3& Vector3::operator/=(float scalar)
{
	return *this = *this / scalar;
}

Vector3& Vector3::operator+=(const Vector3& vector)
{
	return *this = *this + vector;
}

Vector3& Vector3::operator-=(const Vector3& vector)
{
	return *this = *this - vector;
}

bool Vector3::operator==(const Vector3& vector) const
{
	return
		AreEqual(x, vector.x) &&
		AreEqual(y, vector.y) &&
		AreEqual(z, vector.z);
}

float& Vector3::operator[](int index)
{
	switch (index)
	{
	default:
		return x;

	case 1:
		return y;

	case 2:
		return z;
	}
}

float Vector3::operator[](int index) const
{
	switch (index)
	{
	default:
		return x;

	case 1:
		return y;

	case 2:
		return z;
	}
}

Vector3 operator*(float scalar, const Vector3& vector)
{
	return vector * scalar;
}
#pragma endregion



#pragma region Public Methods
float Vector3::GetSquareMagnitude() const
{
	return x * x + y * y + z * z;
}

float Vector3::GetMagnitude() const
{
	return sqrtf(GetSquareMagnitude());
}

Vector3 Vector3::GetNormalized() const
{
	const float magnitude{ GetMagnitude() };
	return Vector3
	(
		x / magnitude,
		y / magnitude,
		z / magnitude
	);
}

const Vector3& Vector3::Normalize()
{
	*this = GetNormalized();
	return *this;
}

Vector2 Vector3::GetVector2() const
{
	return Vector2
	(
		x,
		y
	);
}

Vector4 Vector3::GetVector4() const
{
	return Vector4
	(
		x,
		y,
		z,
		0
	);
}

Vector4 Vector3::GetPoint4() const
{
	return Vector4
	(
		x,
		y,
		z,
		1
	);
}

float Vector3::Dot(const Vector3& vector1, const Vector3& vector2)
{
	return vector1.x * vector2.x + vector1.y * vector2.y + vector1.z * vector2.z;
}

Vector3 Vector3::Cross(const Vector3& vector1, const Vector3& vector2)
{
	return Vector3
	(
		vector1.y * vector2.z - vector1.z * vector2.y,
		vector1.z * vector2.x - vector1.x * vector2.z,
		vector1.x * vector2.y - vector1.y * vector2.x
	);
}

Vector3 Vector3::Project(const Vector3& vector1, const Vector3& vector2)
{
	return vector2 * (Dot(vector1, vector2) / Dot(vector2, vector2));
}

Vector3 Vector3::Reject(const Vector3& vector1, const Vector3& vector2)
{
	return vector1 - Project(vector1, vector2);
}

Vector3 Vector3::Reflect(const Vector3& vector1, const Vector3& vector2)
{
	return vector1 - (2.0f * Dot(vector1, vector2) * vector2);
}
#pragma endregion