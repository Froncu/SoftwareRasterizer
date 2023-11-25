#include <execution>
#include <algorithm>

#include "Constants.hpp"
#include "Renderer.h"
#include "SDL.h"
#include "Vector2.h"

//#define MULTI_THREAD_TRIANGLES
//#define MULTI_THREAD_PIXELS

#pragma region Constructors/Destructor
Renderer::Renderer(SDL_Window* pWindow) :
	m_pWindow{ pWindow },

	m_pFrontBuffer{ SDL_GetWindowSurface(pWindow) },
	m_pBackBuffer{ SDL_CreateRGBSurface(NULL, WINDOW_WIDTH, WINDOW_HEIGHT, 32, NULL, NULL, NULL, NULL) },

	m_pBackBufferPixels{ static_cast<uint32_t*>(m_pBackBuffer->pixels) },

	m_pDepthBufferPixels{ new float[WINDOW_WIDTH * WINDOW_HEIGHT] },

	m_Camera{ Vector3(0.0f, 5.0f, -20.0f), TO_RADIANS * 60.0f },

	m_vMeshes
	{
		Mesh("Resources/tuktuk.obj", "Resources/tuktuk.png"),
	}
{
}

Renderer::~Renderer()
{
	delete[] m_pDepthBufferPixels;
}
#pragma endregion



#pragma region Public Methods
void Renderer::Update(const Timer& timer)
{
	m_Camera.Update(timer);

	for (Mesh& mesh : m_vMeshes)
		mesh.SetRotorY(timer.GetTotal());
}

void Renderer::Render()
{
	SDL_LockSurface(m_pBackBuffer);

	const ColorRGB spaceColor{ GRAY };
	const Uint32 spaceColorMapped
	{
		SDL_MapRGB(m_pBackBuffer->format,
		static_cast<uint8_t>(spaceColor.red * 255),
		static_cast<uint8_t>(spaceColor.green * 255),
		static_cast<uint8_t>(spaceColor.blue * 255))
	};

	std::fill_n(m_pBackBufferPixels, WINDOW_PIXEL_COUNT, spaceColorMapped);
	std::fill_n(m_pDepthBufferPixels, WINDOW_PIXEL_COUNT, INFINITY);

	VertexTransformationFunction(m_vMeshes);

	for (const Mesh& mesh : m_vMeshes)
	{
		const bool triangleStrip{ mesh.GetPrimitiveTopology() == Mesh::PrimitiveTopology::TriangleStrip };

		const std::vector<uint32_t> vIndicesScreen{ mesh.GetIndices() };
		std::vector<VertexOut> vVerticesScreen{ mesh.m_vVerticesOut };

#ifdef MULTI_THREAD_TRIANGLES
		std::vector<size_t> vIndices{};
		vIndices.reserve(vIndicesScreen.size());
		for (size_t index{}; index <= vIndicesScreen.size() - 2; index += triangleStrip ? 1 : 3)
			vIndices.push_back(index);

		std::for_each(std::execution::par, vIndices.begin(), vIndices.end(),
			[this, triangleStrip, &vIndicesScreen, &vVerticesScreen, &mesh]
			(size_t index)
#else
		for (size_t index{}; index < vIndicesScreen.size() - 2; index += triangleStrip ? 1 : 3)
#endif
		{
			const bool isIndexEven{ !(index % 2) };

			VertexOut
				& v0{ vVerticesScreen[vIndicesScreen[index]] },
				& v1{ vVerticesScreen[vIndicesScreen[index + (!triangleStrip ? 1 : isIndexEven ? 1 : 2)]] },
				& v2{ vVerticesScreen[vIndicesScreen[index + (!triangleStrip ? 2 : isIndexEven ? 2 : 1)]] };

			Vector4
				& v0Position{ v0.position },
				& v1Position{ v1.position },
				& v2Position{ v2.position };

			if (v0Position.x < -1.0f || v0Position.x > 1.0f || v0Position.y < -1.0f || v0Position.y > 1.0f || v0Position.z < 0.0f || v0Position.z > 1.0f)
#ifdef MULTI_THREAD_TRIANGLES
				return;
#else
				continue;
#endif

			if (v1Position.x < -1.0f || v1Position.x > 1.0f || v1Position.y < -1.0f || v1Position.y > 1.0f || v1Position.z < 0.0f || v1Position.z > 1.0f)
#ifdef MULTI_THREAD_TRIANGLES
				return;
#else
				continue;
#endif

			if (v2Position.x < -1.0f || v2Position.x > 1.0f || v2Position.y < -1.0f || v2Position.y > 1.0f || v2Position.z < 0.0f || v2Position.z > 1.0f)
#ifdef MULTI_THREAD_TRIANGLES
				return;
#else
				continue;
#endif

			++v0Position.x *= 0.5f * WINDOW_WIDTH;
			v0Position.y = 1.0f - v0Position.y;
			v0Position.y *= 0.5f * WINDOW_HEIGHT;

			++v1Position.x *= 0.5f * WINDOW_WIDTH;
			v1Position.y = 1.0f - v1Position.y;
			v1Position.y *= 0.5f * WINDOW_HEIGHT;

			++v2Position.x *= 0.5f * WINDOW_WIDTH;
			v2Position.y = 1.0f - v2Position.y;
			v2Position.y *= 0.5f * WINDOW_HEIGHT;

			const float
				largestBBX{ std::min(static_cast<float>(WINDOW_WIDTH), std::max(v0Position.x, std::max(v1Position.x, v2Position.x))) },
				largestBBY{ std::min(static_cast<float>(WINDOW_HEIGHT), std::max(v0Position.y, std::max(v1Position.y, v2Position.y))) };

			const int
				smallestBBX{ static_cast<int>(std::max(0.0f, std::min(v0Position.x, std::min(v1Position.x, v2Position.x)))) },
				smallestBBY{ static_cast<int>(std::max(0.0f, std::min(v0Position.y, std::min(v1Position.y, v2Position.y)))) };

#ifdef MULTI_THREAD_PIXELS
			std::vector<float> vPixelsY{};
			for (float py{ smallestBBY + 0.5f }; py < largestBBY; ++py)
				vPixelsY.push_back(py);

			std::for_each(std::execution::par, vPixelsY.begin(), vPixelsY.end(),
				[this, &smallestBBX, &largestBBX, &v0, &v1, &v2, &v0Position, &v1Position, &v2Position, &mesh]
				(float py)
				{
#else
			for (float py{ smallestBBY + 0.5f }; py < largestBBY; ++py)
#endif
				for (float px{ smallestBBX + 0.5f }; px < largestBBX; ++px)
				{
					const Vector2 pixelScreenPosition{ px, py };

					Vector2
						a{ v1Position.GetVector2() - v0Position.GetVector2() },
						c{ pixelScreenPosition - v0Position.GetVector2() };

					const float v2Weight{ Vector2::Cross(a, c) };
					if (v2Weight <= 0.0f)
						continue;

					a = v2Position.GetVector2() - v1Position.GetVector2();
					c = pixelScreenPosition - v1Position.GetVector2();

					const float v0Weight{ Vector2::Cross(a, c) };
					if (v0Weight <= 0.0f)
						continue;

					a = v0Position.GetVector2() - v2Position.GetVector2();
					c = pixelScreenPosition - v2Position.GetVector2();

					const float v1Weight{ Vector2::Cross(a, c) };
					if (v1Weight <= 0.0f)
						continue;

					const float
						totalAreaInversed{ 1.0f / (v0Weight + v1Weight + v2Weight) },
						v0WeightDepthRatio{ v0Weight / v0Position.w * totalAreaInversed },
						v1WeightDepthRatio{ v1Weight / v1Position.w * totalAreaInversed },
						v2WeightDepthRatio{ v2Weight / v2Position.w * totalAreaInversed },

						interpolatedPixelDepth{ 1.0f / (v0WeightDepthRatio + v1WeightDepthRatio + v2WeightDepthRatio) };

					const uint32_t pixelIndex{ static_cast<uint32_t>(px) + (static_cast<uint32_t>(py) * WINDOW_WIDTH) };

					if (interpolatedPixelDepth >= m_pDepthBufferPixels[pixelIndex])
						continue;

					m_pDepthBufferPixels[pixelIndex] = interpolatedPixelDepth;

					const Vector2
						& v0UV{ v0.UVValue },
						& v1UV{ v1.UVValue },
						& v2UV{ v2.UVValue };

					const ColorRGB finalColor
					{
						mesh.GetColorTexture().Sample
						(
							Vector2
							(
								v0UV.x * v0WeightDepthRatio + v1UV.x * v1WeightDepthRatio + v2UV.x * v2WeightDepthRatio,
								v0UV.y * v0WeightDepthRatio + v1UV.y * v1WeightDepthRatio + v2UV.y * v2WeightDepthRatio
							) * interpolatedPixelDepth
						)
					};

					m_pBackBufferPixels[pixelIndex] = SDL_MapRGB(m_pBackBuffer->format,
						static_cast<uint8_t>(finalColor.red * 255),
						static_cast<uint8_t>(finalColor.green * 255),
						static_cast<uint8_t>(finalColor.blue * 255));
				}
#ifdef MULTI_THREAD_PIXELS
				});
#endif
		}
#ifdef MULTI_THREAD_TRIANGLES
	);
#endif
	}

SDL_UnlockSurface(m_pBackBuffer);
SDL_BlitSurface(m_pBackBuffer, nullptr, m_pFrontBuffer, nullptr);
SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}
#pragma endregion



#pragma region Private Methods
void Renderer::VertexTransformationFunction(std::vector<Mesh>& vMeshes) const
{
	const Matrix& cameraMatrix{ m_Camera.GetCameraMatrix() };

	for (Mesh& mesh : vMeshes)
	{
		const Matrix& worldMatrix{ mesh.GetWorldMatrix() };

		const std::vector<Vertex>& vVerticesIn{ mesh.GetVertices() };
		std::vector<VertexOut>& vVerticesOut{ mesh.m_vVerticesOut };

		for (size_t index{}; index < vVerticesIn.size(); ++index)
		{
			const Vertex& vertexIn{ vVerticesIn[index] };
			VertexOut& vertexOut{ vVerticesOut[index] };

			vertexOut.color = vertexIn.color;
			vertexOut.normal = vertexIn.normal;
			vertexOut.tangent = vertexIn.tangent;
			vertexOut.UVValue = vertexIn.UVValue;
			vertexOut.viewDirection = vertexIn.viewDirection;

			Vector4& vertexOutPosition{ vertexOut.position };
			vertexOutPosition = worldMatrix.TransformPoint(vertexIn.position.GetPoint4());
			vertexOutPosition = cameraMatrix.TransformPoint(vertexOutPosition);
			vertexOutPosition.x /= vertexOutPosition.w;
			vertexOutPosition.y /= vertexOutPosition.w;
			vertexOutPosition.z /= vertexOutPosition.w;
		}
	}
}
#pragma endregion

#undef TRIANGLE_LIST
#undef MULTI_THREAD