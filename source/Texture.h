#pragma once

#include <string>

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

	ColorRGB Sample(const Vector2& uv) const;

private:
	std::string m_Path;
	SDL_Surface* m_pSurface;
	uint32_t* m_pSurfacePixels;
};