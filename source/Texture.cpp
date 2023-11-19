#include "Texture.h"

#include "SDL.h"
#include "SDL_Image.h"
#include "Vector2.h"
#include "ColorRGB.h"

#pragma region Constructors/Destructor
Texture::Texture(const std::string& path) :
	m_pSurface{ IMG_Load(path.c_str()) },
	m_pSurfacePixels{ static_cast<uint32_t*>(m_pSurface->pixels) }
{
}

Texture::~Texture()
{
	SDL_FreeSurface(m_pSurface);
}
#pragma endregion



#pragma region Public Methods
ColorRGB Texture::Sample(const Vector2& UVValue) const
{
	if (UVValue.x < 0.0f || UVValue.x > 1.0f || UVValue.y < 0.0f || UVValue.y > 1.0f)
		return ColorRGB{ GRAY };

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