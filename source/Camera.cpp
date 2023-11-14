#include "Camera.h"
#include "SDL_keyboard.h"
#include "SDL_mouse.h"

void Camera::Update(const Timer& timer)
{
	static constexpr float
		MOVEMENT_SPEED{ 15.0f },
		MOUSE_SENSITIVITY{ 0.25f },
		MAX_TOTAL_PITCH{ TO_RADIANS * 90.0f - FLT_EPSILON },
		DEFAULT_FIELD_OF_VIEW_ANGLE{ TO_RADIANS * 45.0f };

	const float
		deltaTime{ timer.GetElapsed() },
		fieldOfViewScalar{ std::min(m_FieldOfViewAngle / DEFAULT_FIELD_OF_VIEW_ANGLE, 1.0f) };

	//	Mouse Input
	int mouseX, mouseY;
	const uint32_t mouseState{ SDL_GetRelativeMouseState(&mouseX, &mouseY) };

	switch (mouseState)
	{
	case SDL_BUTTON(1):
		m_Origin -= m_ForwardDirection * float(mouseY) * deltaTime * MOVEMENT_SPEED * MOUSE_SENSITIVITY;
		m_TotalYaw += TO_RADIANS * fieldOfViewScalar * mouseX * MOUSE_SENSITIVITY;
		UpdateViewMatrix();
		break;

	case SDL_BUTTON(3):
		m_TotalYaw += TO_RADIANS * fieldOfViewScalar * mouseX * MOUSE_SENSITIVITY;
		m_TotalPitch += TO_RADIANS * fieldOfViewScalar * mouseY * MOUSE_SENSITIVITY;
		m_TotalPitch = std::max(-MAX_TOTAL_PITCH, std::min(m_TotalPitch, MAX_TOTAL_PITCH));
		UpdateViewMatrix();
		break;

	case SDL_BUTTON_X2:
		m_Origin -= m_UpDirection * float(mouseY) * deltaTime * MOVEMENT_SPEED * MOUSE_SENSITIVITY;
		UpdateViewMatrix();
		break;
	}

	//	Keyboard Input
	const uint8_t* pKeyboardState{ SDL_GetKeyboardState(nullptr) };

	if (pKeyboardState[SDL_SCANCODE_W])
	{
		m_Origin += m_ForwardDirection * deltaTime * MOVEMENT_SPEED;
		UpdateViewMatrix();
	}

	if (pKeyboardState[SDL_SCANCODE_S])
	{
		m_Origin -= m_ForwardDirection * deltaTime * MOVEMENT_SPEED;
		UpdateViewMatrix();
	}

	if (pKeyboardState[SDL_SCANCODE_A])
	{
		m_Origin -= m_RightDirection * deltaTime * MOVEMENT_SPEED;
		UpdateViewMatrix();
	}

	if (pKeyboardState[SDL_SCANCODE_D])
	{
		m_Origin += m_RightDirection * deltaTime * MOVEMENT_SPEED;
		UpdateViewMatrix();
	}
}