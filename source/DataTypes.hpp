#pragma once

#include <vector>

#include "ColorRGB.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"

struct Int2
{
	int 
		x,
		y;
};

struct Vertex
{
	Vector3 position;
	ColorRGB color;
	//Vector2 uv{}; //W2
	//Vector3 normal{}; //W4
	//Vector3 tangent{}; //W4
	//Vector3 viewDirection{}; //W4
};

struct VertexOut
{
	Vector4 position;
	ColorRGB color;
	//Vector2 uv{};
	//Vector3 normal{};
	//Vector3 tangent{};
	//Vector3 viewDirection{};
};

enum class PrimitiveTopology
{
	TriangleList,
	TriangleStrip
};

struct Mesh
{
	std::vector<Vertex> vVertices{};
	std::vector<uint32_t> vIndices{};
	PrimitiveTopology primitiveTopology{ PrimitiveTopology::TriangleStrip };

	std::vector<VertexOut> vVerticesOut{};
	Matrix worldMatrix;
};