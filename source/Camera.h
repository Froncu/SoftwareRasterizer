#pragma once

#include "Mathematics.hpp"
#include "Timer.h"
#include "Matrix.h"
#include "Vector3.h"

class Camera final
{
public:
	~Camera() = default;

	Camera(const Camera&) = delete;
	Camera(Camera&&) noexcept = delete;
	Camera& operator=(const Camera&) = delete;
	Camera& operator=(Camera&&) noexcept = delete;

	Camera(const Vector3& origin = Vector3(0.0f, 0.0f, 0.0f), float fieldOfViewAngle = TO_RADIANS * 45.0f);

	void Update(const Timer& timer);

	const Matrix& GetInversedViewMatrix() const;
	const Matrix& GetProjectionMatrix() const;
	const Matrix& GetCameraMatrix() const;
	const Vector3& GetOrigin() const;
	float GetFieldOfViewValue() const;

	void SetOrigin(const Vector3& origin);
	void SetFieldOfViewAngle(float angle);
	void IncrementFieldOfViewAngle(float angleIncrementer);

private:
	void UpdateInversedViewMatrix();
	void UpdateProjectionMatrix();

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

	Matrix
		m_InversedViewMatrix,
		m_ProjectionMatrix;
};