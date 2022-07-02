#include "pch.h"
#include "Mesh.h"
#include "VertexBoneData.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

void Mesh::CreateMesh(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices) {}

void Mesh::CreateInstancedMesh(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices) {}

void Mesh::CreateMeshWithNormal(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices) {}

void Mesh::CreateMeshWithTangentNormal(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices) {}

void Mesh::CreateMeshWithBones(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices, std::vector<VertexBoneData>& Bones) {}

void Mesh::RenderMesh() {}

void Mesh::RenderInstancedMesh() {}

void Mesh::RenderTessellatedMesh() {}

Mesh::~Mesh() 
{
	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}
	if (VAO != 0) {
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}
	if (IBO != 0) {
		glDeleteBuffers(1, &IBO);
		IBO = 0;
	}

	indexCount = 0;
	vertexCount = 0;
}