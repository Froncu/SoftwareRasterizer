#pragma once

#include <string>
#include <vector>

#include "Vertex.hpp"
#include "Texture.h"
#include "Matrix.h"

class Mesh final
{
public:
	enum class PrimitiveTopology
	{
		TriangleList,
		TriangleStrip
	};

	~Mesh() = default;

	Mesh(const Mesh& other) = default;
	Mesh(Mesh&&) noexcept = default;
	Mesh& operator=(const Mesh&) = default;
	Mesh& operator=(Mesh&&) noexcept = default;

	Mesh(const std::string& OBJFilePath, const std::string& colorTexturePath, const std::string& normalTexturePath, const std::string& specularTexture, const std::string& glossTexture, bool flipAxisAndWinding = true);

	void SetTranslator(const Vector3& translator);
	void SetRotorY(float yaw);
	void SetScalar(float scalar);

	const std::vector<Vertex>& GetVertices() const;
	const std::vector<uint32_t>& GetIndices() const;
	PrimitiveTopology GetPrimitiveTopology() const;
	const Matrix& GetWorldMatrix() const;
	const Texture& GetColorTexture() const;
	const Texture& GetNormalTexture() const;
	const Texture& GetSpecularTexture() const;
	const Texture& GetGlossTexture() const;

	std::vector<VertexOut> m_vVerticesOut;

private:
	bool ParseOBJ(const std::string& path, bool flipAxisAndWinding);

	std::vector<Vertex> m_vVertices;

	std::vector<uint32_t> m_vIndices;
	PrimitiveTopology m_PrimitiveTopology;

	Matrix
		m_Translator,
		m_Rotor,
		m_Scalar,
		m_WorldMatrix;

	Texture
		m_ColorTexture,
		m_NormalTexture,
		m_SpecularTexture,
		m_GlossTexture;
};