#pragma once

#include <string>

struct SDL_Surface;
struct ColorRGB;
struct Vector2;
class Texture final
{
public:
	~Texture();

	Texture(const Texture&) = delete;
	Texture(Texture&&) noexcept = delete;
	Texture& operator=(const Texture&) = delete;
	Texture& operator=(Texture&&) noexcept = delete;

	Texture(SDL_Surface* pSurface);

	static Texture* LoadFromFile(const std::string& path);

	ColorRGB Sample(const Vector2& uv) const;

private:
	SDL_Surface* m_pSurface;
	uint32_t* m_pSurfacePixels;
};