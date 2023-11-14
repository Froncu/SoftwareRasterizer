#pragma once

#include "Mathematics.hpp"

struct Vector2
{
public:
	float
		x,
		y;

	inline float GetSquareMagnitude() const
	{
		return x * x + y * y;
	}

	inline float GetMagnitude() const
	{
		return sqrtf(GetSquareMagnitude());
	}

	inline Vector2 GetNormalized() const
	{
		const float magnitude{ GetMagnitude() };
		return Vector2
		(
			x / magnitude,
			y / magnitude
		);
	}

	inline const Vector2& Normalize()
	{
		*this = GetNormalized();
		return *this;
	}

	static inline float Dot(const Vector2& vector1, const Vector2& vector2)
	{
		return vector1.x * vector2.x + vector1.y * vector2.y;
	}

	static inline float Cross(const Vector2& vector1, const Vector2& vector2)
	{
		return vector1.x * vector2.y - vector1.y * vector2.x;
	}

	inline Vector2 operator*(float scalar) const
	{
		return Vector2
		(
			x * scalar,
			y * scalar
		);
	}

	inline Vector2 operator/(float scalar) const
	{
		return Vector2
		(
			x / scalar,
			y / scalar
		);
	}

	inline Vector2 operator+(const Vector2& vector) const
	{
		return Vector2
		(
			x + vector.x,
			y + vector.y
		);
	}

	inline Vector2 operator-(const Vector2& vector) const
	{
		return Vector2
		(
			x - vector.x,
			y - vector.y
		);
	}

	inline Vector2 operator-() const
	{
		return Vector2
		(
			-x,
			-y
		);
	}

	inline Vector2& operator*=(float scalar)
	{
		return *this = *this * scalar;
	}

	inline Vector2& operator/=(float scalar)
	{
		return *this = *this / scalar;
	}

	inline Vector2& operator+=(const Vector2& vector)
	{
		return *this = *this + vector;
	}

	inline Vector2& operator-=(const Vector2& vector)
	{
		return *this = *this - vector;
	}

	inline bool operator==(const Vector2& vector) const
	{
		return
			AreEqual(x, vector.x) &&
			AreEqual(y, vector.y);
	}

	inline float& operator[](int index)
	{
		switch (index)
		{
		default:
			return x;

		case 1:
			return y;
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
		}
	}
};

inline Vector2 operator*(float scalar, const Vector2& vector)
{
	return vector * scalar;
}

static constexpr Vector2
VECTOR2_UNIT_X{ 1.0f, 0.0f },
VECTOR2_UNIT_Y{ 0.0f, 1.0f },
VECTOR2_ZERO{ 0.0f, 0.0f };