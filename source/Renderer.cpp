#include <cmath>
#include <iostream>

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

	m_Camera{ Vector3(0.0f, 5.0f, -64.0f), TO_RADIANS * 45.0f },

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
	},

	m_RotateMeshes{ true },
	m_UseNormalTextures{ true },
	m_RenderDepthBuffer{},
	m_InterpolateTexuresBilinearly{ true },

	m_LightingMode{ LightingMode::combined }
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

	static float elapsedTimeSinceStoppedRotating{};

	if (m_RotateMeshes)
		for (Mesh& mesh : m_vMeshes)
			mesh.SetRotorY(timer.GetTotal() - elapsedTimeSinceStoppedRotating);
	else
		elapsedTimeSinceStoppedRotating += timer.GetElapsed();
}

void Renderer::Render()
{
	SDL_LockSurface(m_pBackBuffer);

	ResetBuffers();

	CalculateVerticesOut(m_vMeshes);

	for (Mesh& mesh : m_vMeshes)
	{
		std::vector<VertexOut>& vVerticesOut{ mesh.m_vVerticesOut };
		const std::vector<uint32_t>& vIndices{ mesh.GetIndices() };

		const bool usingTriangleStrip{ mesh.GetPrimitiveTopology() == Mesh::PrimitiveTopology::TriangleStrip };

		for (size_t index{}; index < vIndices.size() - 2; index += usingTriangleStrip ? 1 : 3)
		{
			const bool isIndexEven{ index % 2 == 0 };

			VertexOut
				& v0{ vVerticesOut[vIndices[index]] },
				& v1{ vVerticesOut[vIndices[index + (!usingTriangleStrip ? 1 : isIndexEven ? 1 : 2)]] },
				& v2{ vVerticesOut[vIndices[index + (!usingTriangleStrip ? 2 : isIndexEven ? 2 : 1)]] };

			if (!IsTriangleInFrustum(v0.positionNDC.GetVector3(), v1.positionNDC.GetVector3(), v2.positionNDC.GetVector3()))
				continue;

			Vector2
				v0PositionRaster,
				v1PositionRaster,
				v2PositionRaster;
			NDCToRasterSpace(v0.positionNDC.GetVector3(), v1.positionNDC.GetVector3(), v2.positionNDC.GetVector3(), v0PositionRaster, v1PositionRaster, v2PositionRaster);

			float
				smallestBBX,
				smallestBBY,
				largestBBX,
				largestBBY;
			CalculateBoundingBox(v0PositionRaster, v1PositionRaster, v2PositionRaster, smallestBBX, smallestBBY, largestBBX, largestBBY);

			Vector2 pixelPosition;
			for (float px{ smallestBBX }; px < largestBBX; ++px)
			{
				pixelPosition.x = px;

				for (float py{ smallestBBY }; py < largestBBY; ++py)
				{
					pixelPosition.y = py;

					const uint32_t pixelIndex{ static_cast<uint32_t>(pixelPosition.x) + (static_cast<uint32_t>(pixelPosition.y) * WINDOW_WIDTH) };

					float
						v0Weight,
						v1Weight,
						v2Weight;
					if (!IsPixelInTriangle(pixelPosition, v0PositionRaster, v1PositionRaster, v2PositionRaster, v0Weight, v1Weight, v2Weight))
						continue;

					float
						v0InterpolatedWeight,
						v1InterpolatedWeight,
						v2InterpolatedWeight;
					CalculateInterpolatedWeights(
						v0Weight, v1Weight, v2Weight,
						v0.positionNDC.w, v1.positionNDC.w, v2.positionNDC.w,
						v0InterpolatedWeight, v1InterpolatedWeight, v2InterpolatedWeight);

					float interpolatedPixelDepth;
					if (!DepthTest(pixelIndex, v0InterpolatedWeight, v1InterpolatedWeight, v2InterpolatedWeight, interpolatedPixelDepth))
						continue;

					ColorRGB finalPixelColor;

					if (m_RenderDepthBuffer)
						finalPixelColor = WHITE * ((interpolatedPixelDepth - m_Camera.NEAR_PLANE) / m_Camera.DELTA_NEAR_FAR_PLANE);
					else
					{
						const VertexOut pixelAttributes
						{
							GetPixelAttributes(
							v0, v1, v2,
							v0InterpolatedWeight, v1InterpolatedWeight, v2InterpolatedWeight,
							interpolatedPixelDepth)
						};

						finalPixelColor = GetShadedPixelColor
						(
							pixelAttributes,
							mesh.GetColorTexture(),
							mesh.GetNormalTexture(),
							mesh.GetSpecularTexture(),
							mesh.GetSpecularTexture()
						);
					}

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

void Renderer::ToggleBilinearTextureInterpolation()
{
	m_InterpolateTexuresBilinearly = !m_InterpolateTexuresBilinearly;

	system("CLS");
	std::cout
		<< CONTROLS
		<< "--------\n"
		<< "INTERPOLATED TEXTURE BILINEARLY: " << std::boolalpha << m_InterpolateTexuresBilinearly << std::endl
		<< "--------\n";
}

void Renderer::ToggleRenderDepthBuffer()
{
	m_RenderDepthBuffer = !m_RenderDepthBuffer;

	system("CLS");
	std::cout
		<< CONTROLS
		<< "--------\n"
		<< "RENDER DEPTH BUFFER: " << std::boolalpha << m_RenderDepthBuffer << std::endl
		<< "--------\n";
}

void Renderer::ToggleRotateMeshes()
{
	m_RotateMeshes = !m_RotateMeshes;

	system("CLS");
	std::cout
		<< CONTROLS
		<< "--------\n"
		<< "ROTATION: " << std::boolalpha << m_RotateMeshes << std::endl
		<< "--------\n";
}

void Renderer::ToggleUseNormalTextures()
{
	m_UseNormalTextures = !m_UseNormalTextures;

	system("CLS");
	std::cout
		<< CONTROLS
		<< "--------\n"
		<< "USE NORMAL TEXTURES: " << std::boolalpha << m_UseNormalTextures << std::endl
		<< "--------\n";
}

void Renderer::CycleShadingMode()
{
	m_LightingMode = LightingMode((int(m_LightingMode) + 1) % int(LightingMode::AMOUNT));

	switch (m_LightingMode)
	{
	case Renderer::LightingMode::observedArea:
		system("CLS");
		std::cout
			<< CONTROLS
			<< "--------\n"
			<< "LIGHTING MODE: Observed Area\n"
			<< "--------\n";
		break;

	case Renderer::LightingMode::diffuse:
		system("CLS");
		std::cout
			<< CONTROLS
			<< "--------\n"
			<< "LIGHTING MODE: Diffuse\n"
			<< "--------\n";
		break;

	case Renderer::LightingMode::specular:
		system("CLS");
		std::cout
			<< CONTROLS
			<< "--------\n"
			<< "LIGHTING MODE: Specular\n"
			<< "--------\n";
		break;

	case Renderer::LightingMode::combined:
		system("CLS");
		std::cout
			<< CONTROLS
			<< "--------\n"
			<< "LIGHTING MODE: Combined\n"
			<< "--------\n";
		break;
	}
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}
#pragma endregion



#pragma region Private Methods
void Renderer::ResetBuffers()
{
	static constexpr ColorRGB SPACE_COLOR{ DARK_GRAY };

	std::fill_n(m_pDepthBufferPixels, WINDOW_PIXEL_COUNT, INFINITY);
	std::fill_n(m_pBackBufferPixels, WINDOW_PIXEL_COUNT, SDL_MapRGB(m_pBackBuffer->format,
		static_cast<Uint8>(SPACE_COLOR.red * 255),
		static_cast<Uint8>(SPACE_COLOR.green * 255),
		static_cast<Uint8>(SPACE_COLOR.blue * 255)));
}

void Renderer::CalculateVerticesOut(std::vector<Mesh>& vMeshes) const
{
	const Matrix
		& inversedViewMatrix{ m_Camera.GetInversedViewMatrix() },
		& projectionMatrix{ m_Camera.GetProjectionMatrix() };

	for (Mesh& mesh : vMeshes)
	{
		const Matrix& worldMatrix{ mesh.GetWorldMatrix() };

		const std::vector<VertexLocal>& vVerticesLocal{ mesh.GetVerticesLocal() };
		std::vector<VertexOut>& vVerticesOut{ mesh.m_vVerticesOut };

		for (size_t index{}; index < vVerticesLocal.size(); ++index)
		{
			const VertexLocal& vertexLocal{ vVerticesLocal[index] };
			VertexOut& vertexOut{ vVerticesOut[index] };

			vertexOut.color = vertexLocal.color;
			vertexOut.UV = vertexLocal.UV;

			vertexOut.normal = worldMatrix.TransformVector(vertexLocal.normal);
			vertexOut.tangent = worldMatrix.TransformVector(vertexLocal.tangent);

			const Vector3 vertexOutPositionWorld = worldMatrix.TransformPoint(vertexLocal.position);
			vertexOut.viewDirection = (vertexOutPositionWorld - m_Camera.GetOrigin()).GetNormalized();

			vertexOut.positionNDC = inversedViewMatrix.TransformPoint(vertexOutPositionWorld.GetPoint4());
			vertexOut.positionNDC = projectionMatrix.TransformPoint(vertexOut.positionNDC);

			vertexOut.positionNDC.x /= vertexOut.positionNDC.w;
			vertexOut.positionNDC.y /= vertexOut.positionNDC.w;
			vertexOut.positionNDC.z /= vertexOut.positionNDC.w;
		}
	}
}

bool Renderer::IsTriangleInFrustum(const Vector3& v0Position, const Vector3& v1Position, const Vector3& v2Position)
{
	if (v0Position.x < -1.0f || v0Position.x > 1.0f || v0Position.y < -1.0f || v0Position.y > 1.0f || v0Position.z < 0.0f || v0Position.z > 1.0f)
		return false;

	if (v1Position.x < -1.0f || v1Position.x > 1.0f || v1Position.y < -1.0f || v1Position.y > 1.0f || v1Position.z < 0.0f || v1Position.z > 1.0f)
		return false;

	if (v2Position.x < -1.0f || v2Position.x > 1.0f || v2Position.y < -1.0f || v2Position.y > 1.0f || v2Position.z < 0.0f || v2Position.z > 1.0f)
		return false;

	return true;
}

void Renderer::NDCToRasterSpace(const Vector3& v0PositionNDC, const Vector3& v1PositionNDC, const Vector3& v2PositionNDC, Vector2& v0PositionRaster, Vector2& v1PositionRaster, Vector2& v2PositionRaster)
{
	v0PositionRaster.x = (1.0f + v0PositionNDC.x) * 0.5f * WINDOW_WIDTH;
	v0PositionRaster.y = (1.0f - v0PositionNDC.y) * 0.5f * WINDOW_HEIGHT;

	v1PositionRaster.x = (1.0f + v1PositionNDC.x) * 0.5f * WINDOW_WIDTH;
	v1PositionRaster.y = (1.0f - v1PositionNDC.y) * 0.5f * WINDOW_HEIGHT;

	v2PositionRaster.x = (1.0f + v2PositionNDC.x) * 0.5f * WINDOW_WIDTH;
	v2PositionRaster.y = (1.0f - v2PositionNDC.y) * 0.5f * WINDOW_HEIGHT;
}

void Renderer::CalculateBoundingBox(const Vector2& v0Position, const Vector2& v1Position, const Vector2& v2Position, float& smallestBBX, float& smallestBBY, float& largestBBX, float& largestBBY)
{
	smallestBBX = std::floor(std::max(0.0f, std::min(v0Position.x, std::min(v1Position.x, v2Position.x)))) + 0.5f;
	smallestBBY = std::floor(std::max(0.0f, std::min(v0Position.y, std::min(v1Position.y, v2Position.y)))) + 0.5f;

	largestBBX = std::min(static_cast<float>(WINDOW_WIDTH), std::max(v0Position.x, std::max(v1Position.x, v2Position.x)));
	largestBBY = std::min(static_cast<float>(WINDOW_HEIGHT), std::max(v0Position.y, std::max(v1Position.y, v2Position.y)));
}

bool Renderer::IsPixelInTriangle(const Vector2& pixelPosition, const Vector2& v0Position, const Vector2& v1Position, const Vector2& v2Position, float& v0Weight, float& v1Weight, float& v2Weight)
{
	Vector2
		a{ v1Position - v0Position },
		c{ pixelPosition - v0Position };

	v2Weight = Vector2::Cross(a, c);
	if (v2Weight <= 0.0f)
		return false;

	a = v2Position - v1Position;
	c = pixelPosition - v1Position;

	v0Weight = Vector2::Cross(a, c);
	if (v0Weight <= 0.0f)
		return false;

	a = v0Position - v2Position;
	c = pixelPosition - v2Position;

	v1Weight = Vector2::Cross(a, c);
	if (v1Weight <= 0.0f)
		return false;

	return true;
}

void Renderer::CalculateInterpolatedWeights(float v0Weight, float v1Weight, float v2Weight, float v0CameraDepth, float v1CameraDepth, float v2CameraDepth, float& v0InterpolatedWeight, float& v1InterpolatedWeight, float& v2InterpolatedWeight)
{
	const float totalAreaInversed{ 1.0f / (v0Weight + v1Weight + v2Weight) };

	v0InterpolatedWeight = v0Weight / v0CameraDepth * totalAreaInversed;
	v1InterpolatedWeight = v1Weight / v1CameraDepth * totalAreaInversed;
	v2InterpolatedWeight = v2Weight / v2CameraDepth * totalAreaInversed;
}

bool Renderer::DepthTest(uint32_t pixelIndex, float v0InterpolatedWeight, float v1InterpolatedWeight, float v2InterpolatedWeight, float& interpolatedPixelDepth)
{
	interpolatedPixelDepth = 1.0f / (v0InterpolatedWeight + v1InterpolatedWeight + v2InterpolatedWeight);

	if (interpolatedPixelDepth >= m_pDepthBufferPixels[pixelIndex])
		return false;

	m_pDepthBufferPixels[pixelIndex] = interpolatedPixelDepth;
	return true;
}

VertexOut Renderer::GetPixelAttributes(const VertexOut& v0, const VertexOut& v1, const VertexOut& v2, float v0InterpolatedWeight, float v1InterpolatedWeight, float v2InterpolatedWeight, float interpolatedPixelDepth)
{
	VertexOut pixelAttributes;

	pixelAttributes.positionNDC = v0.positionNDC * v0InterpolatedWeight + v1.positionNDC * v1InterpolatedWeight + v2.positionNDC * v2InterpolatedWeight;
	pixelAttributes.positionNDC *= interpolatedPixelDepth;

	pixelAttributes.color = v0.color * v0InterpolatedWeight + v1.color * v1InterpolatedWeight + v2.color * v2InterpolatedWeight;
	pixelAttributes.color *= interpolatedPixelDepth;

	pixelAttributes.UV = v0.UV * v0InterpolatedWeight + v1.UV * v1InterpolatedWeight + v2.UV * v2InterpolatedWeight;
	pixelAttributes.UV *= interpolatedPixelDepth;

	pixelAttributes.normal = v0.normal * v0InterpolatedWeight + v1.normal * v1InterpolatedWeight + v2.normal * v2InterpolatedWeight;
	pixelAttributes.normal *= interpolatedPixelDepth;
	pixelAttributes.normal.Normalize();

	pixelAttributes.tangent = v0.tangent * v0InterpolatedWeight + v1.tangent * v1InterpolatedWeight + v2.tangent * v2InterpolatedWeight;
	pixelAttributes.tangent *= interpolatedPixelDepth;
	pixelAttributes.tangent.Normalize();

	pixelAttributes.viewDirection = v0.viewDirection * v0InterpolatedWeight + v1.viewDirection * v1InterpolatedWeight + v2.viewDirection * v2InterpolatedWeight;
	pixelAttributes.viewDirection *= interpolatedPixelDepth;
	pixelAttributes.viewDirection.Normalize();

	return pixelAttributes;
}

ColorRGB Renderer::GetShadedPixelColor(const VertexOut& pixelAttributes, const Texture& colorTexture, const Texture& normalTexture, const Texture& specularTexture, const Texture& glossTexture)
{
	static const Vector3 LIGHT_DIRECTION{ 0.577f, -0.577f, 0.577f };
	static constexpr float DIFFUSE_REFLECTANCE{ 7.0f }, SHININESS{ 25.0f };
	static constexpr ColorRGB AMBIENT_COLOR{ 0.03f, 0.03f, 0.03f };

	const Vector2& UV{ pixelAttributes.UV };

	const Vector3& usedNormal{ m_UseNormalTextures ? GetSampledNormal(UV, pixelAttributes.normal, pixelAttributes.tangent, normalTexture) : pixelAttributes.normal };

	const float phongExponent{ SHININESS * glossTexture.Sample(UV, m_InterpolateTexuresBilinearly).red };

	const ColorRGB
		specularReflectance{ specularTexture.Sample(UV, m_InterpolateTexuresBilinearly) },

		diffuse{ Lambert(DIFFUSE_REFLECTANCE, colorTexture.Sample(UV, m_InterpolateTexuresBilinearly)) },
		specular{ Phong(specularReflectance, phongExponent, LIGHT_DIRECTION, pixelAttributes.viewDirection, usedNormal) };

	ColorRGB finalColor{ AMBIENT_COLOR };

	switch (m_LightingMode)
	{
	case Renderer::LightingMode::observedArea:
		finalColor = WHITE;
		break;

	case Renderer::LightingMode::diffuse:
		finalColor += diffuse;
		break;

	case Renderer::LightingMode::specular:
		finalColor += specular;
		break;

	case Renderer::LightingMode::combined:
		finalColor += diffuse + specular;
		break;
	}

	const float dotLightDirectionNormal{ std::max(Vector3::Dot(-LIGHT_DIRECTION, usedNormal), 0.0f) };

	return (dotLightDirectionNormal * finalColor).GetMaxToOne();
}

Vector3 Renderer::GetSampledNormal(const Vector2& UV, const Vector3& normal, const Vector3& tangent, const Texture& normalTexture)
{
	const ColorRGB sampledNormalInColor{ normalTexture.Sample(UV, m_InterpolateTexuresBilinearly) * 2.0f - WHITE };

	const Vector3 binormal{ Vector3::Cross(normal, tangent).GetNormalized() };

	return Matrix
	(
		tangent.GetVector4(),
		binormal.GetVector4(),
		normal.GetVector4(),
		VECTOR4_ZERO
	).TransformVector(Vector3(sampledNormalInColor.red, sampledNormalInColor.green, sampledNormalInColor.blue)).GetNormalized();
}
#pragma endregion