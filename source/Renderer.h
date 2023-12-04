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
	
	void ToggleBilinearTextureInterpolation();
	void ToggleRenderDepthBuffer();
	void ToggleRotateMeshes();
	void ToggleUseNormalTextures();
	void CycleShadingMode();

	bool SaveBufferToImage() const;

	Camera m_Camera;

private:
	void ResetBuffers();

	void CalculateVerticesOut(std::vector<Mesh>& vMeshes) const;

	bool IsTriangleInFrustum(const Vector3& v0Position, const Vector3& v1Position, const Vector3& v2Position);

	void NDCToRasterSpace(const Vector3& v0PositionNDC, const Vector3& v1PositionNDC, const Vector3& v2PositionNDC, Vector2& v0PositionRaster, Vector2& v1PositionRaster, Vector2& v2PositionRaster);

	void CalculateBoundingBox(const Vector2& v0Position, const Vector2& v1Position, const Vector2& v2Position, float& smallestBBX, float& smallestBBY, float& largestBBX, float& largestBBY);

	bool IsPixelInTriangle(const Vector2& pixelPosition, const Vector2& v0Position, const Vector2& v1Position, const Vector2& v2Position, float& v0Weight, float& v1Weight, float& v2Weight);

	void CalculateInterpolatedWeights(float v0Weight, float v1Weight, float v2Weight, float v0CameraDepth, float v1CameraDepth, float v2CameraDepth, float& v0InterpolatedWeight, float& v1InterpolatedWeight, float& v2InterpolatedWeight);

	bool DepthTest(uint32_t pixelIndex, float v0InterpolatedWeight, float v1InterpolatedWeight, float v2InterpolatedWeight, float& interpolatedPixelDepth);

	VertexOut GetPixelAttributes(const VertexOut& v0, const VertexOut& v1, const VertexOut& v2, float v0InterpolatedWeight, float v1InterpolatedWeight, float v2InterpolatedWeight, float interpolatedPixelDepth);

	ColorRGB GetShadedPixelColor(const VertexOut& pixelAttributes, const Texture& colorTexture, const Texture& normalTexture, const Texture& specularTexture, const Texture& glossTexture);

	Vector3 GetSampledNormal(const Vector2& UV, const Vector3& normal, const Vector3& tangent, const Texture& normalTexture);

	SDL_Window* m_pWindow;

	SDL_Surface
		* m_pFrontBuffer, 
		* m_pBackBuffer;

	uint32_t* m_pBackBufferPixels;

	float* m_pDepthBufferPixels;

	std::vector<Mesh> m_vMeshes;

	bool 
		m_RotateMeshes,
		m_UseNormalTextures,
		m_RenderDepthBuffer,
		m_InterpolateTexuresBilinearly;

	enum class LightingMode
	{
		observedArea,
		diffuse,
		specular,
		combined,

		AMOUNT
	} m_LightingMode;
};