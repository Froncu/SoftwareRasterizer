#pragma once

#include "Mathematics.hpp"

struct Vector4
{
public:
	float
		x,
		y,
		z,
		w;

	inline float GetSquareMagnitude() const
	{
		return x * x + y * y + z * z + w * w;
	}

	inline float GetMagnitude() const
	{
		return sqrtf(GetSquareMagnitude());
	}

	inline Vector4 GetNormalized() const
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

	inline const Vector4& Normalize()
	{
		*this = GetNormalized();
		return *this;
	}

	static inline float Dot(const Vector4& vector1, const Vector4& vector2)
	{
		return vector1.x * vector2.x + vector1.y * vector2.y + vector1.z * vector2.z + vector1.w * vector2.w;
	}

	inline Vector4 operator*(float scalar) const
	{
		return Vector4
		(
			x * scalar,
			y * scalar,
			z * scalar,
			w * scalar
		);
	}

	inline Vector4 operator/(float scalar) const
	{
		return Vector4
		(
			x / scalar,
			y / scalar,
			z / scalar,
			w / scalar
		);
	}

	inline Vector4 operator+(const Vector4& vector) const
	{
		return Vector4
		(
			x + vector.x,
			y + vector.y,
			z + vector.z,
			w + vector.w
		);
	}

	inline Vector4 operator-(const Vector4& vector) const
	{
		return Vector4
		(
			x - vector.x,
			y - vector.y,
			z - vector.z,
			w - vector.w
		);
	}

	inline Vector4& operator*=(float scalar)
	{
		return *this = *this * scalar;
	}

	inline Vector4& operator/=(float scalar)
	{
		return *this = *this / scalar;
	}

	inline Vector4& operator+=(const Vector4& vector)
	{
		return *this = *this + vector;
	}

	inline Vector4& operator-=(const Vector4& vector)
	{
		return *this = *this - vector;
	}

	inline bool operator==(const Vector4& vector) const
	{
		return
			AreEqual(x, vector.x) &&
			AreEqual(y, vector.y) &&
			AreEqual(z, vector.z) &&
			AreEqual(w, vector.w);
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

		case 3:
			return w;
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

		case 3:
			return w;
		}
	}
};

inline Vector4 operator*(float scalar, const Vector4& vector)
{
	return vector * scalar;
}

static constexpr Vector4
VECTOR4_UNIT_X{ 1.0f, 0.0f, 0.0f, 0.0f },
VECTOR4_UNIT_Y{ 0.0f, 1.0f, 0.0f, 0.0f },
VECTOR4_UNIT_Z{ 0.0f, 0.0f, 1.0f, 0.0f },
VECTOR4_UNIT_T{ 0.0f, 0.0f, 0.0f, 1.0f },
VECTOR4_ZERO{ 0.0f, 0.0f, 0.0f, 0.0f };