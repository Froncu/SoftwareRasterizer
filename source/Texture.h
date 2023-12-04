#pragma once

#include <string>

#include "SDL_stdinc.h"

struct ColorRGB;
struct Vector2;
struct SDL_Surface;
class Texture final
{
public:
	~Texture();

	Texture(const Texture&);
	Texture(Texture&&) noexcept;
	Texture& operator=(const Texture&);
	Texture& operator=(Texture&&) noexcept;

	Texture(const std::string& path);

	ColorRGB Sample(const Vector2& UV, bool interpolateBilinearly = true) const;

private:
	ColorRGB GetColor(const Vector2& texelPosition) const;

	std::string m_Path;
	SDL_Surface* m_pSurface;
	Uint32* m_pSurfacePixels;
};