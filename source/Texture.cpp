#include "Texture.h"

#include <cmath>

#include "SDL.h"
#include "SDL_Image.h"
#include "Vector2.h"
#include "ColorRGB.h"

#pragma region Constructors/Destructor
Texture::Texture(const std::string& path) :
	m_Path{ path },
	m_pSurface{ IMG_Load(m_Path.c_str()) },
	m_pSurfacePixels{ static_cast<uint32_t*>(m_pSurface->pixels) }
{
}

Texture::Texture(const Texture& other) :
	m_Path{ other.m_Path },
	m_pSurface{ IMG_Load(m_Path.c_str()) },
	m_pSurfacePixels{ static_cast<uint32_t*>(m_pSurface->pixels) }
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
	m_pSurfacePixels = static_cast<uint32_t*>(m_pSurface->pixels);

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
ColorRGB Texture::Sample(const Vector2& UVValue) const
{
	const int
		surfaceWidth{ m_pSurface->w },
		pixelIndexX{ static_cast<int>((surfaceWidth - 1) * UVValue.x) },
		pixelIndexY{ static_cast<int>((m_pSurface->h - 1) * UVValue.y) };

	Uint8
		red,
		green,
		blue;

	SDL_GetRGB(m_pSurfacePixels[pixelIndexX + pixelIndexY * surfaceWidth], m_pSurface->format, &red, &green, &blue);
	return ColorRGB(red / 255.0f, green / 255.0f, blue / 255.0f);
}
#pragma endregion