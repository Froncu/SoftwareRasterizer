#pragma once

struct Vector2;
struct Vector4;
struct Vector3
{
public:
	Vector3 operator*(float scalar) const;
	Vector3 operator/(float scalar) const;
	Vector3 operator+(const Vector3& vector) const;
	Vector3 operator-(const Vector3& vector) const;
	Vector3 operator-() const;

	Vector3& operator*=(float scalar);
	Vector3& operator/=(float scalar);
	Vector3& operator+=(const Vector3& vector);
	Vector3& operator-=(const Vector3& vector);

	bool operator==(const Vector3& vector) const;

	float& operator[](int index);
	float operator[](int index) const;

	static float Dot(const Vector3& vector1, const Vector3& vector2);
	static Vector3 Cross(const Vector3& vector1, const Vector3& vector2);
	static Vector3 Project(const Vector3& vector1, const Vector3& vector2);
	static Vector3 Reject(const Vector3& vector1, const Vector3& vector2);
	static Vector3 Reflect(const Vector3& vector1, const Vector3& vector2);

	float GetSquareMagnitude() const;
	float GetMagnitude() const;

	Vector3 GetNormalized() const;
	const Vector3& Normalize();

	Vector2 GetVector2() const;
	Vector4 GetVector4() const;
	Vector4 GetPoint4() const;

	float
		x,
		y,
		z;
};

Vector3 operator*(float scalar, const Vector3& vector);

static constexpr Vector3
VECTOR3_UNIT_X{ 1.0f, 0.0f, 0.0f },
VECTOR3_UNIT_Y{ 0.0f, 1.0f, 0.0f },
VECTOR3_UNIT_Z{ 0.0f, 0.0f, 1.0f },
VECTOR3_ZERO{ 0.0f, 0.0f, 0.0f };