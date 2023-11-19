#pragma once

struct Vector3;
struct Vector4;
struct Vector2
{
public:
	Vector2 operator*(float scalar) const;
	Vector2 operator/(float scalar) const;
	Vector2 operator+(const Vector2& vector) const;
	Vector2 operator-(const Vector2& vector) const;
	Vector2 operator-() const;

	Vector2& operator*=(float scalar);
	Vector2& operator/=(float scalar);
	Vector2& operator+=(const Vector2& vector);
	Vector2& operator-=(const Vector2& vector);

	bool operator==(const Vector2& vector) const;

	float& operator[](int index);
	float operator[](int index) const;

	static float Dot(const Vector2& vector1, const Vector2& vector2);
	static float Cross(const Vector2& vector1, const Vector2& vector2);

	float GetSquareMagnitude() const;
	float GetMagnitude() const;

	Vector2 GetNormalized() const;
	const Vector2& Normalize();

	Vector3 GetVector3() const;
	Vector4 GetVector4() const;
	Vector4 GetPoint4() const;

	float
		x,
		y;
};

Vector2 operator*(float scalar, const Vector2& vector);

static constexpr Vector2
VECTOR2_UNIT_X{ 1.0f, 0.0f },
VECTOR2_UNIT_Y{ 0.0f, 1.0f },
VECTOR2_ZERO{ 0.0f, 0.0f };