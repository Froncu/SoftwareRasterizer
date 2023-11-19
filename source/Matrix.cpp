#include "Matrix.h"

#include <cassert>

#include "Mathematics.hpp"
#include "Vector3.h"

#pragma region Operators
Matrix Matrix::operator*(const Matrix& matrix) const
{
	Matrix result;
	const Matrix transposedMatrix{ matrix.GetTransposed() };

	for (int currentRow{}; currentRow < 4; ++currentRow)
		for (int currentColumn{}; currentColumn < 4; ++currentColumn)
			result[currentRow][currentColumn] = Vector4::Dot(m_Data[currentRow], transposedMatrix[currentColumn]);

	return result;
}

const Matrix& Matrix::operator*=(const Matrix& matrix)
{
	*this = *this * matrix;
	return *this;
}

bool Matrix::operator==(const Matrix& matrix) const
{
	for (int index{}; index < 4; ++index)
		if (m_Data[index] != matrix[index])
			return false;

	return true;
}

Vector4& Matrix::operator[](int index)
{
	return m_Data[index];
}

const Vector4& Matrix::operator[](int index) const
{
	return m_Data[index];
}
#pragma endregion



#pragma region Public Methods
Matrix Matrix::Inverse(const Matrix& matrix)
{
	return matrix.GetInversed();
}

Matrix Matrix::Transpose(const Matrix& matrix)
{
	return matrix.GetTransposed();
}

Matrix Matrix::CreateTranslator(const Vector3& translator)
{
	return Matrix
	(
		VECTOR4_UNIT_X,
		VECTOR4_UNIT_Y,
		VECTOR4_UNIT_Z,
		translator.GetPoint4()
	);
}

Matrix Matrix::CreateTranslator(float x, float y, float z)
{
	return CreateTranslator(Vector3(x, y, z));
}

Matrix Matrix::CreateRotorX(float pitch)
{
	const float
		cosine{ cosf(pitch) },
		sine{ sinf(pitch) };

	return
		Matrix
		(
			VECTOR4_UNIT_X,
			Vector4(0.0f, cosine, sine, 0.0f),
			Vector4(0.0f, -sine, cosine, 0.0f),
			VECTOR4_UNIT_T
		);
}

Matrix Matrix::CreateRotorY(float yaw)
{
	const float
		cosine{ cosf(yaw) },
		sine{ sinf(yaw) };

	return
		Matrix
		(
			Vector4(cosine, 0.0f, -sine, 0.0f),
			VECTOR4_UNIT_Y,
			Vector4(sine, 0.0f, cosine, 0.0f),
			VECTOR4_UNIT_T
		);
}

Matrix Matrix::CreateRotorZ(float roll)
{
	const float
		cosine{ cosf(roll) },
		sine{ sinf(roll) };

	return
		Matrix
		(
			Vector4(cosine, sine, 0.0f, 0.0f),
			Vector4(-sine, cosine, 0.0f, 0.0f),
			VECTOR4_UNIT_Z,
			VECTOR4_UNIT_T
		);

}

Matrix Matrix::CreateRotor(float pitch, float yaw, float roll)
{
	return
		Matrix::CreateRotorZ(roll) *
		Matrix::CreateRotorY(yaw) *
		Matrix::CreateRotorX(pitch);
}

Matrix Matrix::CreateScalar(float scalarX, float scalarY, float scalarZ)
{
	return
		Matrix
		(
			Vector4(scalarX, 0.0f, 0.0f, 0.0f),
			Vector4(0.0f, scalarY, 0.0f, 0.0f),
			Vector4(0.0f, 0.0f, scalarZ, 0.0f),
			VECTOR4_UNIT_T
		);
}

Matrix Matrix::CreateScalar(float scalar)
{
	return CreateScalar(scalar, scalar, scalar);
}

Matrix Matrix::CreateLookAtLH(const Vector3& origin, const Vector3& forward, const Vector3& up)
{
	//TODO W1

	return {};
}

Matrix Matrix::CreatePerspectiveFovLH(float fovy, float aspect, float zn, float zf)
{
	//TODO W3

	return {};
}

Vector3 Matrix::TransformVector(float x, float y, float z) const
{
	return Vector3
	(
		m_Data[0].x * x + m_Data[1].x * y + m_Data[2].x * z,
		m_Data[0].y * x + m_Data[1].y * y + m_Data[2].y * z,
		m_Data[0].z * x + m_Data[1].z * y + m_Data[2].z * z
	);
}

Vector3 Matrix::TransformVector(const Vector3& vector) const
{
	return TransformVector(vector.x, vector.y, vector.z);
}

Vector3 Matrix::TransformPoint(float x, float y, float z) const
{
	return Vector3
	(
		m_Data[0].x * x + m_Data[1].x * y + m_Data[2].x * z + m_Data[3].x,
		m_Data[0].y * x + m_Data[1].y * y + m_Data[2].y * z + m_Data[3].y,
		m_Data[0].z * x + m_Data[1].z * y + m_Data[2].z * z + m_Data[3].z
	);
}

Vector3 Matrix::TransformPoint(const Vector3& point) const
{
	return TransformPoint(point.x, point.y, point.z);
}

Vector4 Matrix::TransformPoint(float x, float y, float z, float w) const
{
	return Vector4
	(
		m_Data[0].x * x + m_Data[1].x * y + m_Data[2].x * z + m_Data[3].x,
		m_Data[0].y * x + m_Data[1].y * y + m_Data[2].y * z + m_Data[3].y,
		m_Data[0].z * x + m_Data[1].z * y + m_Data[2].z * z + m_Data[3].z,
		m_Data[0].w * w + m_Data[1].w * w + m_Data[2].w * w + m_Data[3].w
	);
}

Vector4 Matrix::TransformPoint(const Vector4& point) const
{
	return TransformPoint(point.x, point.y, point.z, point.w);
}

Matrix Matrix::GetInversed() const
{
	const Vector3
		a{ m_Data[0].GetVector3() },
		b{ m_Data[1].GetVector3() },
		c{ m_Data[2].GetVector3() },
		d{ m_Data[3].GetVector3() };

	const float
		& x{ m_Data[0][3] },
		& y{ m_Data[1][3] },
		& z{ m_Data[2][3] },
		& w{ m_Data[3][3] };

	Vector3
		s{ Vector3::Cross(a, b) },
		t{ Vector3::Cross(c, d) },
		u{ a * y - b * x },
		v{ c * w - d * z };

	const float determinant{ Vector3::Dot(s, v) + Vector3::Dot(t, u) };
	assert((!AreEqual(determinant, 0.0f)) && "ERROR: determinant is 0, there is no INVERSE!");
	const float inverseDeterminant{ 1.0f / determinant };

	s *= inverseDeterminant;
	t *= inverseDeterminant;
	u *= inverseDeterminant;
	v *= inverseDeterminant;

	const Vector3
		r0{ Vector3::Cross(b, v) + t * y },
		r1{ Vector3::Cross(v, a) - t * x },
		r2{ Vector3::Cross(d, u) + s * w },
		r3{ Vector3::Cross(u, c) - s * z };

	return Matrix
	(
		Vector4(r0.x, r1.x, r2.x, 0.0f),
		Vector4(r0.y, r1.y, r2.y, 0.0f),
		Vector4(r0.z, r1.z, r2.z, 0.0f),
		Vector4
		(
			-Vector3::Dot(b, t),
			Vector3::Dot(a, t),
			-Vector3::Dot(d, s),
			Vector3::Dot(c, s)
		)
	);
}

const Matrix& Matrix::Inverse()
{
	*this = GetInversed();
	return *this;
}

Matrix Matrix::GetTransposed() const
{
	Matrix result;
	for (int currentRow{}; currentRow < 4; ++currentRow)
		for (int currentColumn{}; currentColumn < 4; ++currentColumn)
			result[currentRow][currentColumn] = m_Data[currentColumn][currentRow];;

	return result;
}

const Matrix& Matrix::Transpose()
{
	*this = GetTransposed();
	return *this;
}
#pragma endregion