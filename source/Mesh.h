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

	Mesh(const std::string& OBJFilePath, const std::string& diffusePath, bool flipAxisAndWinding = true);

	void ApplyTransforms();

	void SetTranslator(const Vector3& translator);
	void SetRotorY(float yaw);
	void SetScalar(float scalar);

	const std::vector<Vertex>& GetVerticesTransformed() const;
	const std::vector<uint32_t>& GetIndices() const;
	PrimitiveTopology GetPrimitiveTopology() const;
	const Texture& GetColorTexture() const;

private:
	bool ParseOBJ(const std::string& path, bool flipAxisAndWinding);

	std::vector<Vertex> 
		m_vVertices,
		m_vVerticesTranformed;

	std::vector<uint32_t> m_vIndices;
	PrimitiveTopology m_PrimitiveTopology;

	std::vector<VertexOut> m_vVerticesOut;

	Matrix
		m_Translator,
		m_Rotor,
		m_Scalar,
		m_WorldMatrix;

	Texture m_ColorTexture;
};