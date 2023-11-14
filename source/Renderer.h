#pragma once

#include <vector>

#include "DataTypes.hpp"
#include "Camera.h"

struct SDL_Window;
struct SDL_Surface;

class Renderer final
{
public:
	Renderer(SDL_Window* pWindow);
	~Renderer();

	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) noexcept = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer& operator=(Renderer&&) noexcept = delete;

	void Update(const Timer& timer);
	void Render();

	bool SaveBufferToImage() const;

	Camera m_Camera;

private:
	void VertexTransformationFunction(const std::vector<Vertex>& vVerticesIn, std::vector<Vertex>& vVerticesOut) const;

	SDL_Window* m_pWindow;

	SDL_Surface* m_pFrontBuffer;
	SDL_Surface* m_pBackBuffer;
	uint32_t* m_pBackBufferPixels;

	//float* m_pDepthBufferPixels;

	int
		m_Width,
		m_Height;
};