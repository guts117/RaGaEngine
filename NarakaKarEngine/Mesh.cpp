#include "Mesh.h"

#include <iostream>
Mesh::Mesh() {
	VAO = 0;
	VBO = 0;
	IBO = 0;
	indexCount = 0;
	vertexCount = 0;
}

void Mesh::CreateMesh(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices) {

}

void Mesh::CreateMeshNorm(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices)
{
}

void Mesh::CreateAnimatedMesh(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices, std::vector<VertexBoneData>& Bones) {

}

void Mesh::RenderMesh() {
}

void Mesh::RenderAnimatedMesh() {
}
void Mesh::ClearMesh() {
}
Mesh::~Mesh() {
	ClearMesh();
}