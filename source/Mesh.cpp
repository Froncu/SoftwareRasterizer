#include "Mesh.h"

#include <fstream>

#pragma region Constructors/Destructor
Mesh::Mesh(const std::string& OBJFilePath, const std::string& colorTexturePath, const std::string& normalTexturePath, const std::string& specularTexture, const std::string& glossTexture, bool flipAxisAndWinding) :
	m_vVerticesLocal{},
	m_vVerticesOut{},

	m_vIndices{},
	m_PrimitiveTopology{ PrimitiveTopology::TriangleList },

	m_Translator{ IDENTITY },
	m_Rotor{ IDENTITY },
	m_Scalar{ IDENTITY },
	m_WorldMatrix{ IDENTITY },

	m_ColorTexture{ colorTexturePath },
	m_NormalTexture{ normalTexturePath },
	m_SpecularTexture{ specularTexture },
	m_GlossTexture{ glossTexture }
{
	ParseOBJ(OBJFilePath, flipAxisAndWinding);
}
#pragma endregion



#pragma region Public Methods
void Mesh::SetTranslator(const Vector3& translator)
{
	m_Translator = Matrix::CreateTranslator(translator);
	m_WorldMatrix = m_Scalar * m_Rotor * m_Translator;
}

void Mesh::SetRotorY(float yaw)
{
	m_Rotor = Matrix::CreateRotorY(yaw);
	m_WorldMatrix = m_Scalar * m_Rotor * m_Translator;
}

void Mesh::SetScalar(float scalar)
{
	m_Scalar = Matrix::CreateScalar(scalar);
	m_WorldMatrix = m_Scalar * m_Rotor * m_Translator;
}

const std::vector<VertexLocal>& Mesh::GetVerticesLocal() const
{
	return m_vVerticesLocal;
}

const std::vector<uint32_t>& Mesh::GetIndices() const
{
	return m_vIndices;
}

Mesh::PrimitiveTopology Mesh::GetPrimitiveTopology() const
{
	return m_PrimitiveTopology;
}

const Matrix& Mesh::GetWorldMatrix() const
{
	return m_WorldMatrix;
}

const Texture& Mesh::GetColorTexture() const
{
	return m_ColorTexture;
}

const Texture& Mesh::GetNormalTexture() const
{
	return m_NormalTexture;
}

const Texture& Mesh::GetSpecularTexture() const
{
	return m_SpecularTexture;
}

const Texture& Mesh::GetGlossTexture() const
{
	return m_GlossTexture;
}
#pragma endregion



#pragma region Private Methods
bool Mesh::ParseOBJ(const std::string& path, bool flipAxisAndWinding)
{
	std::ifstream file{ path };
	if (!file)
		return false;

	m_vVerticesLocal.clear();
	m_vIndices.clear();

	std::string command;
	std::vector<Vector3> vPositions{};
	std::vector<Vector2> vUVs{};
	std::vector<Vector3> vNormals{};

	while (!file.eof())
	{
		file >> command;

		if (command == "v") // Vertex (Position Local)
		{
			float x, y, z;
			file >> x >> y >> z;
			vPositions.emplace_back(x, y, z);
		}
		else if (command == "vt") // Vertex Texture Coordinate (UV)
		{
			float u, v;
			file >> u >> v;
			vUVs.emplace_back(u, 1.0f - v);
		}
		else if (command == "vn") // Vertex Normal
		{
			float x, y, z;
			file >> x >> y >> z;
			vNormals.emplace_back(x, y, z);
		}
		else if (command == "f") // Face (Triangle)
		{
			std::vector<uint32_t> vTemporaryIndices(3);

			for (size_t faceIndex{}; faceIndex < 3; ++faceIndex)
			{
				VertexLocal vertexLocal{};

				size_t positionIndex;
				file >> positionIndex;
				vertexLocal.position = vPositions[positionIndex - 1]; // OBJ format uses 1-based arrays, hence -1

				if (file.peek() == '/')
				{
					file.ignore();

					if (file.peek() != '/')
					{
						size_t UVIndex;
						file >> UVIndex;
						vertexLocal.UV = vUVs[UVIndex - 1]; // OBJ format uses 1-based arrays, hence -1
					}

					if (file.peek() == '/')
					{
						file.ignore();

						size_t normalIndex;
						file >> normalIndex;
						vertexLocal.normal = vNormals[normalIndex - 1]; // OBJ format uses 1-based arrays, hence -1

						vertexLocal.normal.Normalize();
					}
				}

				m_vVerticesLocal.push_back(vertexLocal);
				vTemporaryIndices[faceIndex] = static_cast<uint32_t>(m_vVerticesLocal.size() - 1);
			}

			m_vIndices.push_back(vTemporaryIndices[0]);
			if (!flipAxisAndWinding)
			{
				m_vIndices.push_back(vTemporaryIndices[1]);
				m_vIndices.push_back(vTemporaryIndices[2]);
			}
			else
			{
				m_vIndices.push_back(vTemporaryIndices[2]);
				m_vIndices.push_back(vTemporaryIndices[1]);
			}
		}

		file.ignore(1000, '\n'); // Read till end of line and ignore all remaining chars
	}

	// Cheap Tangent Calculation
	for (size_t index{}; index < m_vIndices.size(); index += 3)
	{
		const size_t
			index0{ m_vIndices[index] },
			index1{ m_vIndices[index + 1] },
			index2{ m_vIndices[index + 2] };

		const Vector2
			& v0UV{ m_vVerticesLocal[index0].UV },
			& v1UV{ m_vVerticesLocal[index1].UV },
			& v2UV{ m_vVerticesLocal[index2].UV };

		const Vector2
			differenceX{ v1UV.x - v0UV.x, v2UV.x - v0UV.x },
			differenceY{ v1UV.y - v0UV.y, v2UV.y - v0UV.y };

		const float 
			cross{ Vector2::Cross(differenceX, differenceY) },
			inversedCross{ cross ? (1.0f / cross) : 0.0f };

		const Vector3 
			& v0Position{ m_vVerticesLocal[index0].position },
			& v1Position{ m_vVerticesLocal[index1].position },
			& v2Position{ m_vVerticesLocal[index2].position },

			edge0{ v1Position - v0Position },
			edge1{ v2Position - v0Position },
			tangent{ (edge0 * differenceY.y - edge1 * differenceY.x) * inversedCross };

		m_vVerticesLocal[index0].tangent += tangent;
		m_vVerticesLocal[index1].tangent += tangent;
		m_vVerticesLocal[index2].tangent += tangent;
	}

	m_vVerticesOut.resize(m_vVerticesLocal.size());

	// Fix the tangents per vertex now because we accumulated
	for (VertexLocal& vertexLocal : m_vVerticesLocal)
	{
		vertexLocal.tangent = Vector3::Reject(vertexLocal.tangent, vertexLocal.normal).GetNormalized();

		if (flipAxisAndWinding)
		{
			vertexLocal.position.z *= -1.0f;
			vertexLocal.normal.z *= -1.0f;
			vertexLocal.tangent.z *= -1.0f;
		}
	}

	return true;
}
#pragma endregion