#pragma once

#include <vector>

#include "Matrix.hpp"
#include "Vector2.hpp"
#include "ColorRGB.hpp"

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

struct Vertex_Out
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

struct Triangle
{
	const Vertex
		& vertex0,
		& vertex1,
		& vertex2;
};

struct Mesh
{
	std::vector<Vertex> vertices{};
	std::vector<uint32_t> indices{};
	PrimitiveTopology primitiveTopology{ PrimitiveTopology::TriangleStrip };

	std::vector<Vertex_Out> vertices_out{};
	Matrix worldMatrix;
};