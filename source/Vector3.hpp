#pragma once

#include "Mathematics.hpp"

struct Vector3;
inline Vector3 operator*(float scalar, const Vector3& vector);

struct Vector3
{
public:
	float
		x,
		y,
		z;

	inline float GetSquareMagnitude() const
	{
		return x * x + y * y + z * z;
	}

	inline float GetMagnitude() const
	{
		return sqrtf(GetSquareMagnitude());
	}

	inline Vector3 GetNormalized() const
	{
		const float magnitude{ GetMagnitude() };
		return Vector3
		(
			x / magnitude,
			y / magnitude,
			z / magnitude
		);
	}

	inline const Vector3& Normalize()
	{
		*this = GetNormalized();
		return *this;
	}

	static inline float Dot(const Vector3& vector1, const Vector3& vector2)
	{
		return vector1.x * vector2.x + vector1.y * vector2.y + vector1.z * vector2.z;
	}

	static inline Vector3 Cross(const Vector3& vector1, const Vector3& vector2)
	{
		return Vector3
		(
			vector1.y * vector2.z - vector1.z * vector2.y,
			vector1.z * vector2.x - vector1.x * vector2.z,
			vector1.x * vector2.y - vector1.y * vector2.x
		);
	}

	static inline Vector3 Project(const Vector3& vector1, const Vector3& vector2)
	{
		return vector2 * (Dot(vector1, vector2) / Dot(vector2, vector2));
	}

	static inline Vector3 Reject(const Vector3& vector1, const Vector3& vector2)
	{
		return vector1 - Project(vector1, vector2);
	}

	static inline Vector3 Reflect(const Vector3& vector1, const Vector3& vector2)
	{
		return vector1 - (2.0f * Dot(vector1, vector2) * vector2);
	}

	inline Vector3 operator*(float scalar) const
	{
		return Vector3
		(
			x * scalar,
			y * scalar,
			z * scalar
		);
	}

	inline Vector3 operator/(float scalar) const
	{
		return Vector3
		(
			x / scalar,
			y / scalar,
			z / scalar
		);
	}

	inline Vector3 operator+(const Vector3& vector) const
	{
		return Vector3
		(
			x + vector.x,
			y + vector.y,
			z + vector.z
		);
	}

	inline Vector3 operator-(const Vector3& vector) const
	{
		return Vector3
		(
			x - vector.x,
			y - vector.y,
			z - vector.z
		);
	}

	inline Vector3 operator-() const
	{
		return Vector3
		(
			-x,
			-y,
			-z
		);
	}

	inline Vector3& operator*=(float scalar)
	{
		return *this = *this * scalar;
	}

	inline Vector3& operator/=(float scalar)
	{
		return *this = *this / scalar;
	}

	inline Vector3& operator+=(const Vector3& vector)
	{
		return *this = *this + vector;
	}

	inline Vector3& operator-=(const Vector3& vector)
	{
		return *this = *this - vector;
	}

	inline bool operator==(const Vector3& vector) const
	{
		return
			AreEqual(x, vector.x) &&
			AreEqual(y, vector.y) &&
			AreEqual(z, vector.z);
	}

	inline float& operator[](int index)
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

	inline float operator[](int index) const
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
};

inline Vector3 operator*(float scalar, const Vector3& vector)
{
	return vector * scalar;
}

static constexpr Vector3
VECTOR3_UNIT_X{ 1.0f, 0.0f, 0.0f },
VECTOR3_UNIT_Y{ 0.0f, 1.0f, 0.0f },
VECTOR3_UNIT_Z{ 0.0f, 0.0f, 1.0f },
VECTOR3_ZERO{ 0.0f, 0.0f, 0.0f };