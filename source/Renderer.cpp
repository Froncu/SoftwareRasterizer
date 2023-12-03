#include <cmath>

#include "Constants.hpp"
#include "Renderer.h"
#include "SDL.h"
#include "Vector2.h"
#include "BRDFs.hpp"

#pragma region Constructors/Destructor
Renderer::Renderer(SDL_Window* pWindow) :
	m_pWindow{ pWindow },

	m_pFrontBuffer{ SDL_GetWindowSurface(pWindow) },
	m_pBackBuffer{ SDL_CreateRGBSurface(NULL, WINDOW_WIDTH, WINDOW_HEIGHT, 32, NULL, NULL, NULL, NULL) },

	m_pBackBufferPixels{ static_cast<uint32_t*>(m_pBackBuffer->pixels) },

	m_pDepthBufferPixels{ new float[WINDOW_WIDTH * WINDOW_HEIGHT] },

	m_Camera{ Vector3(0.0f, 0.0f, 0.0f), TO_RADIANS * 45.0f },

	m_vMeshes
	{
		Mesh
		(
			"Resources/vehicle.obj",
			"Resources/vehicle_diffuse.png",
			"Resources/vehicle_normal.png",
			"Resources/vehicle_specular.png",
			"Resources/vehicle_gloss.png"
		),
	}
{
	m_vMeshes[0].SetTranslator(Vector3(0.0f, 0.0f, 50.0f));
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

	static constexpr ColorRGB SPACE_COLOR{ GRAY };
	const Uint32 spaceColorMapped
	{
		SDL_MapRGB(m_pBackBuffer->format,
		static_cast<uint8_t>(SPACE_COLOR.red * 255),
		static_cast<uint8_t>(SPACE_COLOR.green * 255),
		static_cast<uint8_t>(SPACE_COLOR.blue * 255))
	};

	std::fill_n(m_pBackBufferPixels, WINDOW_PIXEL_COUNT, spaceColorMapped);
	std::fill_n(m_pDepthBufferPixels, WINDOW_PIXEL_COUNT, INFINITY);

	VertexTransformationFunction(m_vMeshes);

	for (const Mesh& mesh : m_vMeshes)
	{
		const bool triangleStrip{ mesh.GetPrimitiveTopology() == Mesh::PrimitiveTopology::TriangleStrip };

		const std::vector<uint32_t> vIndicesScreen{ mesh.GetIndices() };
		std::vector<VertexOut> vVerticesScreen{ mesh.m_vVerticesOut };

		for (size_t index{}; index < vIndicesScreen.size() - 2; index += triangleStrip ? 1 : 3)
		{
			const bool isIndexEven{ !(index % 2) };

			VertexOut
				& v0{ vVerticesScreen[vIndicesScreen[index]] },
				& v1{ vVerticesScreen[vIndicesScreen[index + (!triangleStrip ? 1 : isIndexEven ? 1 : 2)]] },
				& v2{ vVerticesScreen[vIndicesScreen[index + (!triangleStrip ? 2 : isIndexEven ? 2 : 1)]] };

			if (v0.position.x < -1.0f || v0.position.x > 1.0f || v0.position.y < -1.0f || v0.position.y > 1.0f || v0.position.z < 0.0f || v0.position.z > 1.0f)
				continue;

			if (v1.position.x < -1.0f || v1.position.x > 1.0f || v1.position.y < -1.0f || v1.position.y > 1.0f || v1.position.z < 0.0f || v1.position.z > 1.0f)
				continue;

			if (v2.position.x < -1.0f || v2.position.x > 1.0f || v2.position.y < -1.0f || v2.position.y > 1.0f || v2.position.z < 0.0f || v2.position.z > 1.0f)
				continue;

			++v0.position.x *= 0.5f * WINDOW_WIDTH;
			v0.position.y = 1.0f - v0.position.y;
			v0.position.y *= 0.5f * WINDOW_HEIGHT;

			++v1.position.x *= 0.5f * WINDOW_WIDTH;
			v1.position.y = 1.0f - v1.position.y;
			v1.position.y *= 0.5f * WINDOW_HEIGHT;

			++v2.position.x *= 0.5f * WINDOW_WIDTH;
			v2.position.y = 1.0f - v2.position.y;
			v2.position.y *= 0.5f * WINDOW_HEIGHT;

			const float
				largestBBX{ std::min(static_cast<float>(WINDOW_WIDTH), std::max(v0.position.x, std::max(v1.position.x, v2.position.x))) },
				largestBBY{ std::min(static_cast<float>(WINDOW_HEIGHT), std::max(v0.position.y, std::max(v1.position.y, v2.position.y))) };

			const int
				smallestBBX{ static_cast<int>(std::max(0.0f, std::min(v0.position.x, std::min(v1.position.x, v2.position.x)))) },
				smallestBBY{ static_cast<int>(std::max(0.0f, std::min(v0.position.y, std::min(v1.position.y, v2.position.y)))) };

			Vector2 pixelScreenPosition;
			for (float py{ smallestBBY + 0.5f }; py < largestBBY; ++py)
			{
				pixelScreenPosition.y = py;

				for (float px{ smallestBBX + 0.5f }; px < largestBBX; ++px)
				{
					pixelScreenPosition.x = px;

					Vector2
						a{ v1.position.GetVector2() - v0.position.GetVector2() },
						c{ pixelScreenPosition - v0.position.GetVector2() };

					const float v2Weight{ Vector2::Cross(a, c) };
					if (v2Weight <= 0.0f)
						continue;

					a = v2.position.GetVector2() - v1.position.GetVector2();
					c = pixelScreenPosition - v1.position.GetVector2();

					const float v0Weight{ Vector2::Cross(a, c) };
					if (v0Weight <= 0.0f)
						continue;

					a = v0.position.GetVector2() - v2.position.GetVector2();
					c = pixelScreenPosition - v2.position.GetVector2();

					const float v1Weight{ Vector2::Cross(a, c) };
					if (v1Weight <= 0.0f)
						continue;

					const float
						totalAreaInversed{ 1.0f / (v0Weight + v1Weight + v2Weight) },
						v0WeightDepthRatio{ v0Weight / v0.position.w * totalAreaInversed },
						v1WeightDepthRatio{ v1Weight / v1.position.w * totalAreaInversed },
						v2WeightDepthRatio{ v2Weight / v2.position.w * totalAreaInversed },

						interpolatedPixelDepth{ 1.0f / (v0WeightDepthRatio + v1WeightDepthRatio + v2WeightDepthRatio) };

					const uint32_t pixelIndex{ static_cast<uint32_t>(pixelScreenPosition.x) + (static_cast<uint32_t>(pixelScreenPosition.y) * WINDOW_WIDTH) };

					if (interpolatedPixelDepth >= m_pDepthBufferPixels[pixelIndex])
						continue;

					m_pDepthBufferPixels[pixelIndex] = interpolatedPixelDepth;

					VertexOut pixelAttributes;

					pixelAttributes.position = v0.position * v0WeightDepthRatio + v1.position * v1WeightDepthRatio + v2.position * v2WeightDepthRatio;
					pixelAttributes.position *= interpolatedPixelDepth;

					pixelAttributes.color = v0.color * v0WeightDepthRatio + v1.color * v1WeightDepthRatio + v2.color * v2WeightDepthRatio;
					pixelAttributes.color *= interpolatedPixelDepth;

					pixelAttributes.UVValue = v0.UVValue * v0WeightDepthRatio + v1.UVValue * v1WeightDepthRatio + v2.UVValue * v2WeightDepthRatio;
					pixelAttributes.UVValue *= interpolatedPixelDepth;

					pixelAttributes.normal = v0.normal * v0WeightDepthRatio + v1.normal * v1WeightDepthRatio + v2.normal * v2WeightDepthRatio;
					pixelAttributes.normal *= interpolatedPixelDepth;

					pixelAttributes.tangent = v0.tangent * v0WeightDepthRatio + v1.tangent * v1WeightDepthRatio + v2.tangent * v2WeightDepthRatio;
					pixelAttributes.tangent *= interpolatedPixelDepth;

					pixelAttributes.viewDirection = v0.viewDirection * v0WeightDepthRatio + v1.viewDirection * v1WeightDepthRatio + v2.viewDirection * v2WeightDepthRatio;
					pixelAttributes.viewDirection *= interpolatedPixelDepth;

					const ColorRGB finalPixelColor
					{ 
						PixelShading
						(
							pixelAttributes, 
							mesh.GetColorTexture(), 
							mesh.GetNormalTexture(), 
							mesh.GetSpecularTexture(),
							mesh.GetSpecularTexture()
						).GetMaxToOne() 
					};

					m_pBackBufferPixels[pixelIndex] = SDL_MapRGB(m_pBackBuffer->format,
						static_cast<uint8_t>(finalPixelColor.red * 255),
						static_cast<uint8_t>(finalPixelColor.green * 255),
						static_cast<uint8_t>(finalPixelColor.blue * 255));
				}
			}
		}
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
			vertexOut.UVValue = vertexIn.UVValue;
			vertexOut.normal = worldMatrix.TransformVector(vertexIn.normal);
			vertexOut.tangent = worldMatrix.TransformVector(vertexIn.tangent);

			vertexOut.position = worldMatrix.TransformPoint(vertexIn.position.GetPoint4());
			vertexOut.viewDirection = (m_Camera.GetOrigin() - vertexOut.position.GetVector3()).GetNormalized();

			vertexOut.position = cameraMatrix.TransformPoint(vertexOut.position);

			vertexOut.position.x /= vertexOut.position.w;
			vertexOut.position.y /= vertexOut.position.w;
			vertexOut.position.z /= vertexOut.position.w;
		}
	}
}

ColorRGB Renderer::PixelShading(const VertexOut& pixelAttributes, const Texture& colorTexture, const Texture& normalTexture, const Texture& specularTexture, const Texture& glossTexture)
{
	static constexpr Vector3 LIGHT_DIRECTION{ -0.577f, 0.577f, -0.577f };
	static constexpr float 
		LIGHT_INTENSITY{ 7.0f },
		SHININESS{ 25.0f };
	static constexpr ColorRGB AMBIENT_COLOR{ 0.025f, 0.025f, 0.025f };

	ColorRGB sampledNormalInColor{ normalTexture.Sample(pixelAttributes.UVValue) };
	sampledNormalInColor = 2.0f * sampledNormalInColor - WHITE;
	Vector3 sampledNormal{ sampledNormalInColor.red, sampledNormalInColor.green, sampledNormalInColor.blue };

	sampledNormal = Matrix
	(
		pixelAttributes.tangent.GetVector4(),
		Vector3::Cross(pixelAttributes.normal, pixelAttributes.tangent).GetNormalized().GetVector4(),
		pixelAttributes.normal.GetVector4(),
		VECTOR4_ZERO
	).TransformVector(sampledNormal);

	const float
		dotLightDirectionNormal{ std::max(Vector3::Dot(LIGHT_DIRECTION, sampledNormal), 0.0f) },
		specularReflectance{ glossTexture.Sample(pixelAttributes.UVValue).red },
		phongExponent{ SHININESS * specularTexture.Sample(pixelAttributes.UVValue).red };

	return
		dotLightDirectionNormal *
		(Lambert(LIGHT_INTENSITY, colorTexture.Sample(pixelAttributes.UVValue)) +
		Phong(specularReflectance, phongExponent, LIGHT_DIRECTION, pixelAttributes.viewDirection, sampledNormal) +
		AMBIENT_COLOR);
}
#pragma endregion