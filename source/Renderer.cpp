#include <execution>
#include <algorithm>

#include "Renderer.h"
#include "SDL.h"
#include "Vector2.h"

//#define MULTI_THREAD_TRIANGLES
//#define MULTI_THREAD_PIXELS

#pragma region Constructors/Destructor
Renderer::Renderer(SDL_Window* pWindow) :
	m_pWindow{ pWindow },

	m_pFrontBuffer{ SDL_GetWindowSurface(pWindow) },

	m_Camera{ Vector3(0.0f, 5.0f, -20.0f), TO_RADIANS * 60.0f },

	m_vWorldMeshes
	{
		Mesh("Resources/tuktuk.obj", "Resources/tuktuk.png"),
		Mesh("Resources/tuktuk.obj", "Resources/tuktuk.png")
	}
{
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_AspectRatio = static_cast<float>(m_Width) / m_Height;

	m_pBackBuffer = SDL_CreateRGBSurface(NULL, m_Width, m_Height, 32, NULL, NULL, NULL, NULL);
	m_pBackBufferPixels = static_cast<uint32_t*>(m_pBackBuffer->pixels);
	m_pDepthBufferPixels = new float[m_Width * m_Height];

	m_vWorldMeshes[0].SetTranslator(Vector3(10.0f, 0.0f, 0.0f));
	m_vWorldMeshes[0].ApplyTransforms();

	m_vWorldMeshes[1].SetTranslator(Vector3(-10.0f, 0.0f, 0.0f));
	m_vWorldMeshes[1].ApplyTransforms();
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

	for (Mesh& mesh : m_vWorldMeshes)
	{
		mesh.SetRotorY(timer.GetTotal());
		mesh.ApplyTransforms();
	}
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

	std::fill_n(m_pBackBufferPixels, m_Width * m_Height, spaceColorMapped);
	std::fill_n(m_pDepthBufferPixels, m_Width * m_Height, INFINITY);

	for (const Mesh& meshWorld : m_vWorldMeshes)
	{
		std::vector<Vertex> vVerticesScreen{};
		VertexTransformationFunction(meshWorld.GetVerticesTransformed(), vVerticesScreen);

		const bool triangleStrip{ meshWorld.GetPrimitiveTopology() == Mesh::PrimitiveTopology::TriangleStrip };

		const std::vector<uint32_t>& vIndices{ meshWorld.GetIndices() };
#ifdef MULTI_THREAD_TRIANGLES
		std::for_each(std::execution::par, vIndices.begin(), vIndices.end(), [this, triangleStrip, &vIndices, &vVerticesScreen, &meshWorld]
		(uint32_t value)
		{
				const size_t index
				{
					static_cast<size_t>(std::find(vIndices.begin(), vIndices.end(), value) - vIndices.begin()) *
					(triangleStrip ? 1 : 3)
				};

				if (index >= vIndices.size() - 2)
					return;
#else
		for (size_t index{}; index < vIndices.size() - 2; index += triangleStrip ? 1 : 3)
		{
#endif
			const bool isIndexEven{ !(index % 2) };

			const Vertex
				& v0{ vVerticesScreen[vIndices[index]] },
				& v1{ vVerticesScreen[vIndices[index + (!triangleStrip ? 1 : isIndexEven ? 1 : 2)]] },
				& v2{ vVerticesScreen[vIndices[index + (!triangleStrip ? 2 : isIndexEven ? 2 : 1)]] };

			const Vector3
				& v0Position{ v0.position },
				& v1Position{ v1.position },
				& v2Position{ v2.position };

			const float
				largestBBX{ std::min(static_cast<float>(m_Width), std::max(v0Position.x, std::max(v1Position.x, v2Position.x))) },
				largestBBY{ std::min(static_cast<float>(m_Height), std::max(v0Position.y, std::max(v1Position.y, v2Position.y))) };

			const int
				smallestBBX{ static_cast<int>(std::max(0.0f, std::min(v0Position.x, std::min(v1Position.x, v2Position.x)))) },
				smallestBBY{ static_cast<int>(std::max(0.0f, std::min(v0Position.y, std::min(v1Position.y, v2Position.y)))) };

#ifdef MULTI_THREAD_PIXELS
			std::vector<float> vPixelsY{};
			for (float py{ smallestBBY + 0.5f }; py < largestBBY; ++py)
				vPixelsY.push_back(py);

			std::for_each(std::execution::par, vPixelsY.begin(), vPixelsY.end(),
				[this, &smallestBBX, &largestBBX, &v0, &v1, &v2, &v0Position, &v1Position, &v2Position, &meshWorld]
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
						v0WeightDepthRatio{ v0Weight / v0Position.z * totalAreaInversed },
						v1WeightDepthRatio{ v1Weight / v1Position.z * totalAreaInversed },
						v2WeightDepthRatio{ v2Weight / v2Position.z * totalAreaInversed },

						interpolatedPixelDepth{ 1.0f / (v0WeightDepthRatio + v1WeightDepthRatio + v2WeightDepthRatio) };

					const int pixelIndex{ static_cast<int>(px) + (static_cast<int>(py) * m_Width) };

					if (interpolatedPixelDepth >= m_pDepthBufferPixels[pixelIndex])
						continue;

					m_pDepthBufferPixels[pixelIndex] = interpolatedPixelDepth;

					const Vector2
						& v0UV{ v0.UVValue },
						& v1UV{ v1.UVValue },
						& v2UV{ v2.UVValue };

					const ColorRGB finalColor
					{
						meshWorld.GetColorTexture().Sample
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

#ifdef MULTI_THREAD_TRIANGLES
		});
#else
		}
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
void Renderer::VertexTransformationFunction(const std::vector<Vertex>& vVerticesIn, std::vector<Vertex>& vVerticesOut) const
{
	const Matrix& viewMatrix{ m_Camera.GetViewMatrix() };
	const float fieldOfViewValue{ m_Camera.GetFieldOfViewValue() };
	const int size{ static_cast<int>(vVerticesIn.size()) };

	vVerticesOut.resize(size);

	for (int index{}; index < size; ++index)
	{
		const Vertex& vertexIn{ vVerticesIn[index] };
		Vertex& vertexOut{ vVerticesOut[index] };

		vertexOut.color = vertexIn.color;
		vertexOut.UVValue = vertexIn.UVValue;

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
#pragma endregion

#undef TRIANGLE_LIST
#undef MULTI_THREAD