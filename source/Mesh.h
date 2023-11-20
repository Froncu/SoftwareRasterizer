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

	const std::vector<Vertex>& GetVertices() const;
	const std::vector<uint32_t>& GetIndices() const;

	PrimitiveTopology GetPrimitiveTopology() const;

	const Texture& GetDiffuse() const;

private:
	bool ParseOBJ(const std::string& path, bool flipAxisAndWinding);

	std::vector<Vertex> m_vVertices;
	std::vector<uint32_t> m_vIndices;
	PrimitiveTopology m_PrimitiveTopology;

	std::vector<VertexOut> m_vVerticesOut;
	Matrix m_WorldMatrix;

	Texture m_Diffuse;
};