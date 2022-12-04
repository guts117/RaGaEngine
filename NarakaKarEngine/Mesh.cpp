#include "pch.h"
#include "Mesh.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

//ToDo: Add Support for Render with Static and Dynamic Batching.
struct Mesh::Impl
{
	GLuint m_MaterialIndex, m_IndexCnt, m_VAO, m_VBO, m_IBO, m_VBO_bones, m_InstanceCnt;
	bool m_IsTesselated;

	Impl() = delete;

	Impl(Impl&& rhs) noexcept = default;
	Impl& operator=(Impl&& rhs) noexcept = default;

	Impl(const Impl& rhs) noexcept = delete;
	Impl& operator=(const Impl& rhs) noexcept = delete;

	Impl(GLuint&& materialIndex, std::vector<std::vector<GLfloat>>&& vertices2D, std::vector<GLuint>&& indices, MeshGenParams&& meshGenParams)
		: m_MaterialIndex{ std::move(materialIndex) }
		, m_IndexCnt{ (GLuint) indices.size() }
		, m_VAO { 0 }
		, m_VBO { 0 }
		, m_IBO { 0 }
		, m_VBO_bones { 0 }
		, m_InstanceCnt { std::move(meshGenParams.InstanceCount) }
		, m_IsTesselated { std::move(meshGenParams.IsTessellated) }
	{
		auto vertex1D = std::vector<GLfloat>();

		for (auto vertex : vertices2D) 
		{
			for (auto val : vertex) 
			{
				vertex1D.push_back(val);
			}
		}
		
		auto vertexAttribStride = vertices2D[0].size() * sizeof(vertex1D[0]);
		auto vertexBufferSize = sizeof(vertex1D[0]) * vertex1D.size();

		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		glGenBuffers(1, &m_IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * m_IndexCnt, &indices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, &vertex1D[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexAttribStride, 0);    //vertex position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexAttribStride, (void*)(sizeof(vertex1D[0]) * 3));  //texture co-ordinates
		glEnableVertexAttribArray(1);

		if (meshGenParams.HasNormal)
		{
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertexAttribStride, (void*)(sizeof(vertex1D[0]) * 5));  //normal co-ordinates
			glEnableVertexAttribArray(2);
		}

		if (meshGenParams.HasTangent)
		{
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, vertexAttribStride, (void*)(sizeof(vertex1D[0]) * 8));  //tangent co-ordinates
			glEnableVertexAttribArray(3);
		}

		if (meshGenParams.BoneData.size() > 0)
		{
			glGenBuffers(1, &m_VBO_bones);
			glBindBuffer(GL_ARRAY_BUFFER, m_VBO_bones);
			glBufferData(GL_ARRAY_BUFFER, meshGenParams.BoneData.size() * sizeof(meshGenParams.BoneData[0]), &meshGenParams.BoneData[0], GL_STATIC_DRAW);

			glVertexAttribIPointer(4, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);
			glEnableVertexAttribArray(5);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void RenderMesh()
	{
		if (m_IsTesselated)
		{
			glBindVertexArray(m_VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
			glDrawElements(GL_PATCHES, m_IndexCnt, GL_UNSIGNED_INT, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		else if (m_InstanceCnt)
		{
			glBindVertexArray(m_VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
			glDrawElementsInstanced(GL_TRIANGLE_STRIP, m_IndexCnt, GL_UNSIGNED_INT, 0, m_InstanceCnt);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		else
		{
			glBindVertexArray(m_VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
			glDrawElements(GL_TRIANGLES, m_IndexCnt, GL_UNSIGNED_INT, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
	}

	~Impl()
	{
		if (m_VBO != 0) {
			glDeleteBuffers(1, &m_VBO);
		}
		if (m_VAO != 0) {
			glDeleteVertexArrays(1, &m_VAO);
		}
		if (m_IBO != 0) {
			glDeleteBuffers(1, &m_IBO);
		}

		if (m_VBO_bones != 0) {
			glDeleteBuffers(1, &m_VBO_bones);
		}
	}
};

Mesh::Mesh(GLuint&& materialIndex, std::vector<std::vector<GLfloat>>&& vertices, std::vector<GLuint>&& indices, MeshGenParams&& meshGenParams)
	: m_pImpl { std::make_unique<Impl>(std::move(materialIndex),std::move(vertices),std::move(indices), std::move(meshGenParams))}
{
}

Mesh::Mesh(Mesh&& rhs) = default;
Mesh& Mesh::operator= (Mesh&& rhs) = default;

void Mesh::RenderMesh() 
{
	Pimpl()->RenderMesh();
}

const GLuint& Mesh::GetMaterialIndex() const
{
	return Pimpl()->m_MaterialIndex;
}

//ToDo: Add Support for Render with Static and Dynamic Batching.

Mesh::~Mesh() = default;