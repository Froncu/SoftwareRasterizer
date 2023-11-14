#pragma once

#include <cfloat>
#include <cmath>

#include "Constants.hpp"
#include "Matrix.hpp"
#include "Timer.h"

class Camera final
{
public:
	Camera(const Vector3& origin = Vector3(0.0f, 0.0f, 0.0f), float fieldOfViewAngle = TO_RADIANS * 45.0f) :
		m_Origin{ origin },
		m_ForwardDirection{ VECTOR3_UNIT_Z },
		m_RightDirection{ VECTOR3_UNIT_X },
		m_UpDirection{ VECTOR3_UNIT_Y },

		m_FieldOfViewAngle{ fieldOfViewAngle },
		m_FieldOfViewValue{ tanf(m_FieldOfViewAngle / 2.0f) },

		m_TotalPitch{},
		m_TotalYaw{},

		m_ViewMatrix{}
	{
		UpdateViewMatrix();
		UpdateProjectionMatrix();
	}

	inline const Matrix& GetViewMatrix() const
	{
		return m_ViewMatrix;
	}

	inline const Vector3& GetOrigin() const
	{
		return m_Origin;
	}

	inline float GetFieldOfViewValue() const
	{
		return m_FieldOfViewValue;
	}

	inline void SetOrigin(const Vector3& origin)
	{
		m_Origin = origin;
	}

	inline void SetFieldOfViewAngle(float angle)
	{
		static const float MAX_FOV_ANGLE{ TO_RADIANS * 180.0f - FLT_EPSILON };
		m_FieldOfViewAngle = std::max(FLT_EPSILON, std::min(angle, MAX_FOV_ANGLE));
		m_FieldOfViewValue = tanf(m_FieldOfViewAngle / 2.0f);
	}

	inline void IncrementFieldOfViewAngle(float angleIncrementer)
	{
		SetFieldOfViewAngle(m_FieldOfViewAngle + angleIncrementer);
	}

	void Update(const Timer& timer);

private:
	inline void UpdateViewMatrix()
	{
		static constexpr Vector3 WORLD_UP{ 0.0f, 1.0f, 0.0f };
		m_ForwardDirection = Matrix(Matrix::CreateRotorX(m_TotalPitch) * Matrix::CreateRotorY(m_TotalYaw)).TransformVector(VECTOR3_UNIT_Z);
		m_RightDirection = Vector3::Cross(WORLD_UP, m_ForwardDirection).GetNormalized();
		m_UpDirection = Vector3::Cross(m_ForwardDirection, m_RightDirection).GetNormalized();

		m_ViewMatrix = Matrix
		(
			GetVector4(m_RightDirection),
			GetVector4(m_UpDirection),
			GetVector4(m_ForwardDirection),
			GetPoint4(m_Origin)
		).GetInversed();

		//ViewMatrix => Matrix::CreateLookAtLH(...) [not implemented yet]
		//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatlh
	}

	inline void UpdateProjectionMatrix()
	{
		//TODO W3

		//ProjectionMatrix => Matrix::CreatePerspectiveFovLH(...) [not implemented yet]
		//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovlh
	}

	Vector3
		m_Origin,
		m_ForwardDirection,
		m_RightDirection,
		m_UpDirection;

	float
		m_FieldOfViewAngle,
		m_FieldOfViewValue,

		m_TotalPitch,
		m_TotalYaw;

	Matrix m_ViewMatrix;
};