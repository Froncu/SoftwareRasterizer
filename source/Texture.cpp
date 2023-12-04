#include "Texture.h"

#include "SDL_image.h"
#include "Vector2.h"
#include "ColorRGB.h"
#include "Mathematics.hpp"

#pragma region Constructors/Destructor
Texture::Texture(const std::string& path) :
	m_Path{ path },
	m_pSurface{ IMG_Load(m_Path.c_str()) },
	m_pSurfacePixels{ static_cast<Uint32*>(m_pSurface->pixels) }
{
}

Texture::Texture(const Texture& other) :
	m_Path{ other.m_Path },
	m_pSurface{ IMG_Load(m_Path.c_str()) },
	m_pSurfacePixels{ static_cast<Uint32*>(m_pSurface->pixels) }
{
}

Texture::Texture(Texture&& other) noexcept :
	m_Path{ other.m_Path },
	m_pSurface{ other.m_pSurface },
	m_pSurfacePixels{ other.m_pSurfacePixels }
{
	other.m_pSurface = nullptr;
	other.m_pSurfacePixels = nullptr;
}

Texture::~Texture()
{
	SDL_FreeSurface(m_pSurface);
}
#pragma endregion



#pragma region Operators
Texture& Texture::operator=(const Texture& other)
{
	SDL_FreeSurface(m_pSurface);

	m_Path = other.m_Path;
	m_pSurface = IMG_Load(m_Path.c_str());
	m_pSurfacePixels = static_cast<Uint32*>(m_pSurface->pixels);

	return *this;
}

Texture& Texture::operator=(Texture&& other) noexcept
{
	SDL_FreeSurface(m_pSurface);

	m_Path = other.m_Path;
	m_pSurface = other.m_pSurface;
	m_pSurfacePixels = other.m_pSurfacePixels;

	other.m_pSurface = nullptr;
	other.m_pSurfacePixels = nullptr;

	return *this;
}
#pragma endregion



#pragma region Public Methods
ColorRGB Texture::Sample(const Vector2& UV, bool interpolateBilinearly) const
{
	const float
		U{ std::fmodf(UV.x, 1.0f) },
		V{ std::fmodf(UV.y, 1.0f) };

	const Vector2 texelPosition
	{
		U * (m_pSurface->w - 1),
		V * (m_pSurface->h - 1)
	};

	if (!interpolateBilinearly)
	{
		return GetColor(texelPosition);
	}
	else
	{
		Vector2
			texelPositionIntegralPart,
			texelPositionFractionalPart
		{
			std::modf(texelPosition.x, &texelPositionIntegralPart.x),
			std::modf(texelPosition.y, &texelPositionIntegralPart.y)
		};

		const ColorRGB
			color00{ GetColor(texelPositionIntegralPart) },
			color10{ GetColor(texelPositionIntegralPart + VECTOR2_UNIT_X) },
			color01{ GetColor(texelPositionIntegralPart + VECTOR2_UNIT_Y) },
			color11{ GetColor(texelPositionIntegralPart + VECTOR2_UNIT_X + VECTOR2_UNIT_Y) };

		return Lerp
		(
			Lerp(color00, color10, texelPositionFractionalPart.x),
			Lerp(color01, color11, texelPositionFractionalPart.x),
			texelPositionFractionalPart.y
		);
	}
}
#pragma endregion



#pragma region Private Methods
ColorRGB Texture::GetColor(const Vector2& texelPosition) const
{
	Uint8
		red,
		green,
		blue;

	SDL_LockSurface(m_pSurface);
	SDL_GetRGB(
		m_pSurfacePixels[static_cast<int>(texelPosition.x) + static_cast<int>(texelPosition.y) * m_pSurface->w], 
		m_pSurface->format, 
		&red, &green, &blue);
	SDL_UnlockSurface(m_pSurface);

	return ColorRGB
	(
		red / 255.0f,
		green / 255.0f,
		blue / 255.0f
	);
}
#pragma endregion