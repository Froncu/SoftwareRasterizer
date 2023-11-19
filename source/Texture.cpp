#include "Texture.h"

#include "SDL.h"
#include "Vector2.h"
#include "ColorRGB.h"

#pragma region Constructors/Destructor
Texture::Texture(SDL_Surface* pSurface) :
	m_pSurface{ pSurface },
	m_pSurfacePixels{ (uint32_t*)pSurface->pixels }
{
}

Texture::~Texture()
{
	if (m_pSurface)
	{
		SDL_FreeSurface(m_pSurface);
		m_pSurface = nullptr;
	}
}
#pragma endregion



#pragma region Public Methods
Texture* Texture::LoadFromFile(const std::string& path)
{
	//TODO
	//Load SDL_Surface using IMG_LOAD
	//Create & Return a new Texture Object (using SDL_Surface)

	return nullptr;
}

ColorRGB Texture::Sample(const Vector2& uv) const
{
	//TODO
	//Sample the correct texel for the given uv

	return {};
}
#pragma endregion