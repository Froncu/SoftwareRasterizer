#pragma once

#include "Vector4.h"

struct Vector3;
class Matrix final
{
public:
	~Matrix() = default;

	Matrix(const Matrix&) = default;
	Matrix(Matrix&&) noexcept = default;
	Matrix& operator=(const Matrix&) = default;
	Matrix& operator=(Matrix&&) noexcept = default;

	Matrix() = default;
	constexpr Matrix(const Vector4& xAxis, const Vector4& yAxis, const Vector4& zAxis, const Vector4& translator) :
		m_Data{ xAxis, yAxis, zAxis, translator }
	{
	}

	Matrix operator*(const Matrix& matrix) const;

	const Matrix& operator*=(const Matrix& matrix);

	bool operator==(const Matrix& matrix) const;

	Vector4& operator[](int index);
	const Vector4& operator[](int index) const;

	static Matrix Inverse(const Matrix& matrix);
	static Matrix Transpose(const Matrix& matrix);

	static Matrix CreateTranslator(const Vector3& translator);
	static Matrix CreateTranslator(float x, float y, float z);

	static Matrix CreateRotorX(float pitch);
	static Matrix CreateRotorY(float yaw);
	static Matrix CreateRotorZ(float roll);
	static Matrix CreateRotor(float pitch, float yaw, float roll);

	static Matrix CreateScalar(float scalarX, float scalarY, float scalarZ);
	static Matrix CreateScalar(float scalar);

	static Matrix CreateLookAtLH(const Vector3& origin, const Vector3& forward, const Vector3& up);
	static Matrix CreatePerspectiveFovLH(float fovy, float aspect, float zn, float zf);

	Vector3 TransformVector(float x, float y, float z) const;
	Vector3 TransformVector(const Vector3& vector) const;

	Vector3 TransformPoint(float x, float y, float z) const;
	Vector3 TransformPoint(const Vector3& point) const;
	Vector4 TransformPoint(float x, float y, float z, float w) const;
	Vector4 TransformPoint(const Vector4& point) const;

	Matrix GetInversed() const;
	const Matrix& Inverse();

	Matrix GetTransposed() const;
	const Matrix& Transpose();

private:
	Vector4 m_Data[4];
};

static constexpr Matrix IDENTITY
{
	VECTOR4_UNIT_X,
	VECTOR4_UNIT_Y,
	VECTOR4_UNIT_Z,
	VECTOR4_UNIT_T
};