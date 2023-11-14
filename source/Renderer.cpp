#include "Renderer.h"
#include "SDL.h"
#include "Texture.h"
#include "Utilities.hpp"

Renderer::Renderer(SDL_Window* pWindow) :
	m_pWindow{ pWindow },

	m_pFrontBuffer{ SDL_GetWindowSurface(pWindow) },

	m_Camera{ Vector3(0.0f, 0.0f, -10.0f), TO_RADIANS * 60.0f },

	m_vVerticesWorld
	{
		Vertex(Vector3(0.0f, 2.0f, 0.0f), RED),
		Vertex(Vector3(1.5f, -1.0f, 0.0f), GREEN),
		Vertex(Vector3(-1.5f, -1.0f, 0.0f), BLUE),

		Vertex(Vector3(0.0f, 4.0f, 2.0f), RED),
		Vertex(Vector3(3.0f, -2.0f, 2.0f), GREEN),
		Vertex(Vector3(-3.0f, -2.0f, 2.0f), BLUE)
	}
{
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_AspectRatio = static_cast<float>(m_Width) / m_Height;

	m_pBackBuffer = SDL_CreateRGBSurface(NULL, m_Width, m_Height, 32, NULL, NULL, NULL, NULL);
	m_pBackBufferPixels = static_cast<uint32_t*>(m_pBackBuffer->pixels);
	m_pDepthBufferPixels = new float[m_Width * m_Height];
}

Renderer::~Renderer()
{
	delete[] m_pDepthBufferPixels;
}

void Renderer::Update(const Timer& timer)
{
	m_Camera.Update(timer);
}

void Renderer::Render()
{
	SDL_LockSurface(m_pBackBuffer);
	
	memset(m_pBackBufferPixels, 0, m_Width * m_Height * sizeof(uint32_t));
	for (int index{}; index < m_Width * m_Height; ++index)
		//m_pBackBufferPixels[index] = 0,
		m_pDepthBufferPixels[index] = INFINITY;

	std::vector<Vertex> vVerticesScreen{};
	VertexTransformationFunction(m_vVerticesWorld, vVerticesScreen);

	for (size_t triangleIndex{}; triangleIndex < vVerticesScreen.size(); triangleIndex += 3)
	{
		const Vertex
			& v0{ vVerticesScreen[triangleIndex] },
			& v1{ vVerticesScreen[triangleIndex + 1] },
			& v2{ vVerticesScreen[triangleIndex + 2] };

		const Vector3
			& v0Position{ v0.position },
			& v1Position{ v1.position },
			& v2Position{ v2.position };

		const int
			largestBBX{ std::min(m_Width, static_cast<int>(std::max(v0Position.x, std::max(v1Position.x, v2Position.x)))) },
			largestBBY{ std::min(m_Height, static_cast<int>(std::max(v0Position.y, std::max(v1Position.y, v2Position.y)))) },
			smallestBBX{ static_cast<int>(std::max(0.0f, std::min(v0Position.x, std::min(v1Position.x, v2Position.x)))) },
			smallestBBY{ static_cast<int>(std::max(0.0f, std::min(v0Position.y, std::min(v1Position.y, v2Position.y)))) };

		for (float px{ smallestBBX + 0.5f }; px < largestBBX; ++px)
			for (float py{ smallestBBY + 0.5f }; py < largestBBY; ++py)
			{
				const Vector2 pixelPosition{ px, py };

				Vector2
					a{ GetVector2(v1Position) - GetVector2(v0Position) },
					c{ pixelPosition - GetVector2(v0Position) };

				const float v2SignedArea{ Vector2::Cross(a, c) };
				if (v2SignedArea < 0.0f)
					continue;

				a = GetVector2(v2Position) - GetVector2(v1Position);
				c = pixelPosition - GetVector2(v1Position);

				const float v0SignedArea{ Vector2::Cross(a, c) };
				if (v0SignedArea < 0.0f)
					continue;

				a = GetVector2(v0Position) - GetVector2(v2Position);
				c = pixelPosition - GetVector2(v2Position);

				const float v1SignedArea{ Vector2::Cross(a, c) };
				if (v1SignedArea < 0.0f)
					continue;

				const float
					totalArea{ v0SignedArea + v1SignedArea + v2SignedArea },
					totalAreaInversed{ 1.0f / totalArea },

					v0Ratio{ v0SignedArea * totalAreaInversed },
					v1Ratio{ v1SignedArea * totalAreaInversed },
					v2Ratio{ v2SignedArea * totalAreaInversed },

					pixelDepth{ v0Position.z * v0Ratio + v1Position.z * v1Ratio + v2Position.z * v2Ratio };

				const int pixelIndex{ static_cast<int>(px) + (static_cast<int>(py) * m_Width) };

				if (pixelDepth >= m_pDepthBufferPixels[pixelIndex])
					continue;

				m_pDepthBufferPixels[pixelIndex] = pixelDepth;

				const ColorRGB finalColor{ (v0.color * v0Ratio + v1.color * v1Ratio + v2.color * v2Ratio).GetMaxToOne() };

				m_pBackBufferPixels[pixelIndex] = SDL_MapRGB(m_pBackBuffer->format,
					static_cast<uint8_t>(finalColor.red * 255),
					static_cast<uint8_t>(finalColor.green * 255),
					static_cast<uint8_t>(finalColor.blue * 255));
			}
	}

	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, nullptr, m_pFrontBuffer, nullptr);
	SDL_UpdateWindowSurface(m_pWindow);
}

void Renderer::VertexTransformationFunction(const std::vector<Vertex>& vVerticesIn, std::vector<Vertex>& vVerticesOut) const
{
	const Matrix& viewMatrix{ m_Camera.GetViewMatrix() };
	const float fieldOfViewValue{ m_Camera.GetFieldOfViewValue() };
	const int size{ static_cast<int>(vVerticesIn.size()) };

	vVerticesOut.resize(size);

	for (int index{}; index < size; ++index)
	{
		vVerticesOut[index].color = vVerticesIn[index].color;

		Vector3& vertexOutPosition{ vVerticesOut[index].position };

		vertexOutPosition = viewMatrix.TransformPoint(vVerticesIn[index].position);

		vertexOutPosition.x = vertexOutPosition.x / vertexOutPosition.z;
		vertexOutPosition.y = vertexOutPosition.y / vertexOutPosition.z;

		vertexOutPosition.x /= m_AspectRatio * fieldOfViewValue;
		vertexOutPosition.y /= fieldOfViewValue;

		vertexOutPosition = Vector3
		(
			(vertexOutPosition.x + 1.0f) * 0.5f * m_Width,
			(1.0f - vertexOutPosition.y) * 0.5f * m_Height,
			vertexOutPosition.z
		);
	}
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}
