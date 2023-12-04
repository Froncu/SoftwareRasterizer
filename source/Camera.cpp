#include <iostream>

#include "Camera.h"
#include "SDL_keyboard.h"
#include "SDL_mouse.h"
#include "Constants.hpp"

#pragma region Constructors/Destructor
Camera::Camera(const Vector3& origin, float fieldOfViewAngle) :
	m_Origin{ origin },
	m_ForwardDirection{ VECTOR3_UNIT_Z },
	m_RightDirection{ VECTOR3_UNIT_X },
	m_UpDirection{ VECTOR3_UNIT_Y },

	m_FieldOfViewAngle{ fieldOfViewAngle },
	m_FieldOfViewValue{ tanf(m_FieldOfViewAngle / 2.0f) },

	m_TotalPitch{},
	m_TotalYaw{},

	m_InversedViewMatrix{},
	m_ProjectionMatrix{}
{
	UpdateInversedViewMatrix();
	UpdateProjectionMatrix();
}
#pragma endregion



#pragma region Static Data Members
const float
Camera::NEAR_PLANE{ 0.1f },
Camera::FAR_PLANE{ 100.0f },
Camera::DELTA_NEAR_FAR_PLANE{ FAR_PLANE - NEAR_PLANE };
#pragma endregion



#pragma region Public Methods
void Camera::Update(const Timer& timer)
{
	static constexpr float
		MOVEMENT_SPEED{ 15.0f },
		MOUSE_SENSITIVITY{ 0.25f },
		MAX_TOTAL_PITCH{ TO_RADIANS * 90.0f - FLT_EPSILON },
		DEFAULT_FIELD_OF_VIEW_ANGLE{ TO_RADIANS * 45.0f },
		MOUSE_MOVEMENT_ORIGIN_CORRECTOR{ 0.0625f };

	const float
		deltaTime{ timer.GetElapsed() },
		fieldOfViewScalar{ std::min(m_FieldOfViewAngle / DEFAULT_FIELD_OF_VIEW_ANGLE, 1.0f) };

	//	Mouse Input
	int mouseX, mouseY;
	const uint32_t mouseState{ SDL_GetRelativeMouseState(&mouseX, &mouseY) };

	switch (mouseState)
	{
	case SDL_BUTTON(1):
		m_Origin -= m_ForwardDirection * MOVEMENT_SPEED * float(mouseY) * MOUSE_SENSITIVITY * MOUSE_MOVEMENT_ORIGIN_CORRECTOR;
		m_TotalYaw += TO_RADIANS * fieldOfViewScalar * mouseX * MOUSE_SENSITIVITY;
		UpdateInversedViewMatrix();
		break;

	case SDL_BUTTON(3):
		m_TotalYaw += TO_RADIANS * fieldOfViewScalar * mouseX * MOUSE_SENSITIVITY;
		m_TotalPitch += TO_RADIANS * fieldOfViewScalar * mouseY * MOUSE_SENSITIVITY;
		m_TotalPitch = std::max(-MAX_TOTAL_PITCH, std::min(m_TotalPitch, MAX_TOTAL_PITCH));
		UpdateInversedViewMatrix();
		break;

	case SDL_BUTTON_X2:
		m_Origin -= m_UpDirection * MOVEMENT_SPEED * float(mouseY) * MOUSE_SENSITIVITY * MOUSE_MOVEMENT_ORIGIN_CORRECTOR;
		UpdateInversedViewMatrix();
		break;
	}

	//	Keyboard Input
	const uint8_t* pKeyboardState{ SDL_GetKeyboardState(nullptr) };

	if (pKeyboardState[SDL_SCANCODE_W] || pKeyboardState[SDL_SCANCODE_UP])
	{
		m_Origin += m_ForwardDirection * deltaTime * MOVEMENT_SPEED;
		UpdateInversedViewMatrix();
	}

	if (pKeyboardState[SDL_SCANCODE_S] || pKeyboardState[SDL_SCANCODE_DOWN])
	{
		m_Origin -= m_ForwardDirection * deltaTime * MOVEMENT_SPEED;
		UpdateInversedViewMatrix();
	}

	if (pKeyboardState[SDL_SCANCODE_A] || pKeyboardState[SDL_SCANCODE_LEFT])
	{
		m_Origin -= m_RightDirection * deltaTime * MOVEMENT_SPEED;
		UpdateInversedViewMatrix();
	}

	if (pKeyboardState[SDL_SCANCODE_D] || pKeyboardState[SDL_SCANCODE_RIGHT])
	{
		m_Origin += m_RightDirection * deltaTime * MOVEMENT_SPEED;
		UpdateInversedViewMatrix();
	}
}
#pragma endregion



#pragma region Getters
const Matrix& Camera::GetInversedViewMatrix() const
{
	return m_InversedViewMatrix;
}

const Matrix& Camera::GetProjectionMatrix() const
{
	return m_ProjectionMatrix;
}

const Vector3& Camera::GetOrigin() const
{
	return m_Origin;
}

float Camera::GetFieldOfViewValue() const
{
	return m_FieldOfViewValue;
}
#pragma endregion



#pragma region Setters
void Camera::SetOrigin(const Vector3& origin)
{
	m_Origin = origin;
}

void Camera::SetFieldOfViewAngle(float angle)
{
	static const float MAX_FOV_ANGLE{ TO_RADIANS * 180.0f - FLT_EPSILON };
	m_FieldOfViewAngle = std::max(FLT_EPSILON, std::min(angle, MAX_FOV_ANGLE));
	m_FieldOfViewValue = tanf(m_FieldOfViewAngle / 2.0f);

	system("CLS");
	std::cout
		<< CONTROLS
		<< "--------\n"
		<< "FIELD OF VIEW ANGLE: " << TO_DEGREES * m_FieldOfViewAngle << " degrees\n"
		<< "--------\n";

	UpdateProjectionMatrix();
}

void Camera::IncrementFieldOfViewAngle(float angleIncrementer)
{
	SetFieldOfViewAngle(m_FieldOfViewAngle + angleIncrementer);
}
#pragma endregion



#pragma region Private Methods
void Camera::UpdateInversedViewMatrix()
{
	static constexpr Vector3 WORLD_UP{ 0.0f, 1.0f, 0.0f };
	m_ForwardDirection = Matrix(Matrix::CreateRotorX(m_TotalPitch) * Matrix::CreateRotorY(m_TotalYaw)).TransformVector(VECTOR3_UNIT_Z);
	m_RightDirection = Vector3::Cross(WORLD_UP, m_ForwardDirection).GetNormalized();
	m_UpDirection = Vector3::Cross(m_ForwardDirection, m_RightDirection).GetNormalized();

	m_InversedViewMatrix = Matrix
	(
		m_RightDirection.GetVector4(),
		m_UpDirection.GetVector4(),
		m_ForwardDirection.GetVector4(),
		m_Origin.GetPoint4()
	).GetInversed();

	//ViewMatrix => Matrix::CreateLookAtLH(...) [not implemented yet]
	//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatlh
}

void Camera::UpdateProjectionMatrix()
{
	static const Vector4
		Z_AXIS{ 0.0f, 0.0f, FAR_PLANE / DELTA_NEAR_FAR_PLANE, 1.0f },
		TRANSLATOR{ 0.0f, 0.0f, -FAR_PLANE * NEAR_PLANE / DELTA_NEAR_FAR_PLANE, 0.0f };

	m_ProjectionMatrix = Matrix
	(
		Vector4(1.0f / (ASPECT_RATIO * m_FieldOfViewValue), 0.0f, 0.0f, 0.0f),
		Vector4(0.0f, 1.0f / m_FieldOfViewValue, 0.0f, 0.0f),
		Z_AXIS,
		TRANSLATOR
	);

	//ProjectionMatrix => Matrix::CreatePerspectiveFovLH(...) [not implemented yet]
	//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovlh
}
#pragma endregion