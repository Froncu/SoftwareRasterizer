#pragma once

struct Vector2;
struct Vector3;
struct Vector4
{
public:
	Vector4 operator*(float scalar) const;
	Vector4 operator/(float scalar) const;
	Vector4 operator+(const Vector4& vector) const;
	Vector4 operator-(const Vector4& vector) const;

	Vector4& operator*=(float scalar);
	Vector4& operator/=(float scalar);
	Vector4& operator+=(const Vector4& vector);
	Vector4& operator-=(const Vector4& vector);

	bool operator==(const Vector4& vector) const;

	float& operator[](int index);
	float operator[](int index) const;

	static float Dot(const Vector4& vector1, const Vector4& vector2);

	float GetSquareMagnitude() const;
	float GetMagnitude() const;

	Vector4 GetNormalized() const;
	const Vector4& Normalize();

	Vector2 GetVector2() const;
	Vector3 GetVector3() const;

	float
		x,
		y,
		z,
		w;

};

Vector4 operator*(float scalar, const Vector4& vector);

static constexpr Vector4
VECTOR4_UNIT_X{ 1.0f, 0.0f, 0.0f, 0.0f },
VECTOR4_UNIT_Y{ 0.0f, 1.0f, 0.0f, 0.0f },
VECTOR4_UNIT_Z{ 0.0f, 0.0f, 1.0f, 0.0f },
VECTOR4_UNIT_T{ 0.0f, 0.0f, 0.0f, 1.0f },
VECTOR4_ZERO{ 0.0f, 0.0f, 0.0f, 0.0f };