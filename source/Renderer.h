#pragma once

#include <vector>

#include "Camera.h"
#include "Mesh.h"

struct SDL_Window;
struct SDL_Surface;

class Renderer final
{
public:
	~Renderer();

	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) noexcept = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer& operator=(Renderer&&) noexcept = delete;
	
	Renderer(SDL_Window* pWindow);

	void Update(const Timer& timer);
	void Render();

	bool SaveBufferToImage() const;

	Camera m_Camera;

private:
	void VertexTransformationFunction(std::vector<Mesh>& vMeshes) const;

	SDL_Window* m_pWindow;

	SDL_Surface
		* m_pFrontBuffer, 
		* m_pBackBuffer;

	uint32_t* m_pBackBufferPixels;

	float* m_pDepthBufferPixels;

	std::vector<Mesh> m_vMeshes;
};