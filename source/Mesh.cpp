#include "Mesh.h"

#include <fstream>

#pragma region Constructors/Destructor
Mesh::Mesh(const std::string& OBJFilePath, const std::string& diffusePath, bool flipAxisAndWinding) :
	m_vVertices{},
	m_vIndices{},
	m_PrimitiveTopology{ PrimitiveTopology::TriangleList },

	m_vVerticesOut{},
	m_WorldMatrix{},

	m_Diffuse{ diffusePath }
{
	ParseOBJ(OBJFilePath, flipAxisAndWinding);
}
#pragma endregion



#pragma region Public Methods
const std::vector<Vertex>& Mesh::GetVertices() const
{
	return m_vVertices;
}

const std::vector<uint32_t>& Mesh::GetIndices() const
{
	return m_vIndices;
}

Mesh::PrimitiveTopology Mesh::GetPrimitiveTopology() const
{
	return m_PrimitiveTopology;
}

const Texture& Mesh::GetDiffuse() const
{
	return m_Diffuse;
}
#pragma endregion



#pragma region Private Methods
bool Mesh::ParseOBJ(const std::string& path, bool flipAxisAndWinding)
{
	std::ifstream file(path);
	if (!file)
		return false;

	std::vector<Vector3> positions{};
	std::vector<Vector3> normals{};
	std::vector<Vector2> UVs{};

	m_vVertices.clear();
	m_vIndices.clear();

	std::string sCommand;
	// start a while iteration ending when the end of file is reached (ios::eof)
	while (!file.eof())
	{
		//read the first word of the string, use the >> operator (istream::operator>>) 
		file >> sCommand;
		//use conditional statements to process the different commands	
		if (sCommand == "#")
		{
			// Ignore Comment
		}
		else if (sCommand == "v")
		{
			//Vertex
			float x, y, z;
			file >> x >> y >> z;

			positions.emplace_back(x, y, z);
		}
		else if (sCommand == "vt")
		{
			// Vertex TexCoord
			float u, v;
			file >> u >> v;
			UVs.emplace_back(u, 1 - v);
		}
		else if (sCommand == "vn")
		{
			// Vertex Normal
			float x, y, z;
			file >> x >> y >> z;

			normals.emplace_back(x, y, z);
		}
		else if (sCommand == "f")
		{
			//if a face is read:
			//construct the 3 vertices, add them to the vertex array
			//add three indices to the index array
			//add the material index as attibute to the attribute array
			//
			// Faces or triangles
			Vertex vertex{};
			size_t iPosition, iTexCoord, iNormal;

			uint32_t tempIndices[3];
			for (size_t iFace = 0; iFace < 3; iFace++)
			{
				// OBJ format uses 1-based arrays
				file >> iPosition;
				vertex.position = positions[iPosition - 1];

				if ('/' == file.peek())//is next in buffer ==  '/' ?
				{
					file.ignore();//read and ignore one element ('/')

					if ('/' != file.peek())
					{
						// Optional texture coordinate
						file >> iTexCoord;
						vertex.UVValue = UVs[iTexCoord - 1];
					}

					if ('/' == file.peek())
					{
						file.ignore();

						// Optional vertex normal
						file >> iNormal;
						vertex.normal = normals[iNormal - 1];
					}
				}

				m_vVertices.push_back(vertex);
				tempIndices[iFace] = uint32_t(m_vVertices.size()) - 1;
				//indices.push_back(uint32_t(vertices.size()) - 1);
			}

			m_vIndices.push_back(tempIndices[0]);
			if (flipAxisAndWinding)
			{
				m_vIndices.push_back(tempIndices[2]);
				m_vIndices.push_back(tempIndices[1]);
			}
			else
			{
				m_vIndices.push_back(tempIndices[1]);
				m_vIndices.push_back(tempIndices[2]);
			}
		}
		//read till end of line and ignore all remaining chars
		file.ignore(1000, '\n');
	}

	//Cheap Tangent Calculations
	for (uint32_t i = 0; i < m_vIndices.size(); i += 3)
	{
		uint32_t index0 = m_vIndices[i];
		uint32_t index1 = m_vIndices[size_t(i) + 1];
		uint32_t index2 = m_vIndices[size_t(i) + 2];

		const Vector3& p0 = m_vVertices[index0].position;
		const Vector3& p1 = m_vVertices[index1].position;
		const Vector3& p2 = m_vVertices[index2].position;
		const Vector2& uv0 = m_vVertices[index0].UVValue;
		const Vector2& uv1 = m_vVertices[index1].UVValue;
		const Vector2& uv2 = m_vVertices[index2].UVValue;

		const Vector3 edge0 = p1 - p0;
		const Vector3 edge1 = p2 - p0;
		const Vector2 diffX = Vector2(uv1.x - uv0.x, uv2.x - uv0.x);
		const Vector2 diffY = Vector2(uv1.y - uv0.y, uv2.y - uv0.y);
		float r = 1.f / Vector2::Cross(diffX, diffY);

		Vector3 tangent = (edge0 * diffY.y - edge1 * diffY.x) * r;
		m_vVertices[index0].tangent += tangent;
		m_vVertices[index1].tangent += tangent;
		m_vVertices[index2].tangent += tangent;
	}

	//Fix the tangents per vertex now because we accumulated
	for (auto& v : m_vVertices)
	{
		v.tangent = Vector3::Reject(v.tangent, v.normal).GetNormalized();

		if (flipAxisAndWinding)
		{
			v.position.z *= -1.f;
			v.normal.z *= -1.f;
			v.tangent.z *= -1.f;
		}
	}

	return true;
}
#pragma endregion