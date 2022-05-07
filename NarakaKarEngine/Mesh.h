#pragma once
#include <GL/glew.h>
#include "CommonValues.h"
#include <vector>

#include "glm\glm.hpp"
#include "assimp\Importer.hpp"
#include "VertexBoneData.h"

class Mesh
{
public:
	Mesh();
	virtual void CreateMesh(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices);
	virtual void CreateMeshNorm(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices);
	virtual void CreateAnimatedMesh(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices, std::vector<VertexBoneData>& Bones);
	virtual void RenderMesh();
	virtual void RenderAnimatedMesh();
	glm::mat4 prevMesh = glm::mat4(1.0);
	~Mesh();

protected:
	virtual void ClearMesh();

	GLuint indexCount, vertexCount;
	std::vector<VertexBoneData> Bones;

	GLuint VAO, VBO, IBO;
	GLuint VBO_bones;
};

