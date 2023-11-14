#include "Renderer.h"
#include "SDL.h"
#include "Texture.h"
#include "Utilities.hpp"

Renderer::Renderer(SDL_Window* pWindow) :
	m_pWindow{ pWindow },

	m_pFrontBuffer{ SDL_GetWindowSurface(pWindow) },

	m_Camera{ Vector3(0.0f, 0.0f, -10.0f), TO_RADIANS * 60.0f }
{
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

	m_pBackBuffer = SDL_CreateRGBSurface(NULL, m_Width, m_Height, 32, NULL, NULL, NULL, NULL);
	m_pBackBufferPixels = static_cast<uint32_t*>(m_pBackBuffer->pixels);
	//m_pDepthBufferPixels = new float[m_Width * m_Height];
}

Renderer::~Renderer()
{
	//delete[] m_pDepthBufferPixels;
}

void Renderer::Update(const Timer& timer)
{
	m_Camera.Update(timer);
}

void Renderer::Render()
{
	SDL_LockSurface(m_pBackBuffer);

	for (float px{ 0.5f }; px < m_Width; ++px)
		for (float py{ 0.5f }; py < m_Height; ++py)
		{
			float gradient{ px / m_Width };
			gradient += py / m_Width;
			gradient /= 2.0f;

			ColorRGB finalColor{ gradient, gradient, gradient };

			finalColor.MaxToOne();

			m_pBackBufferPixels[static_cast<int>(px) + (static_cast<int>(py) * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
				static_cast<uint8_t>(finalColor.red * 255),
				static_cast<uint8_t>(finalColor.green * 255),
				static_cast<uint8_t>(finalColor.blue * 255));
		}

	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, nullptr, m_pFrontBuffer, nullptr);
	SDL_UpdateWindowSurface(m_pWindow);
}

void Renderer::VertexTransformationFunction(const std::vector<Vertex>& vVerticesIn, std::vector<Vertex>& vVerticesOut) const
{
	//Todo > W1 Projection Stage
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}
