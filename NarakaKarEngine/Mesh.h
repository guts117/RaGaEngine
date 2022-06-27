#ifndef MESH
#define MESH

#include "pch.h"

class VertexBoneData;

class Mesh
{
public:
	explicit Mesh() = default;
	
	Mesh(Mesh&& rhs) = default;
	Mesh& operator= (Mesh&& rhs) = default;

	Mesh(const Mesh& rhs) = delete;
	Mesh& operator= (const Mesh& rhs) = delete;
	
	virtual void CreateMesh(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices);
	virtual void CreateInstancedMesh(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices);
	virtual void CreateMeshWithNormal(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices);
	virtual void CreateMeshWithTangentNormal(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices);
	virtual void CreateMeshWithBones(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices, std::vector<VertexBoneData>& Bones);
	virtual void RenderMesh();
	virtual void RenderInstancedMesh();
	virtual void RenderTessellatedMesh();

	glm::mat4 PrevMesh = glm::mat4(1.0);

	virtual ~Mesh() = 0;

protected:
	GLuint indexCount = 0, vertexCount = 0, VAO = 0, VBO = 0, IBO = 0;
};

#endif