#pragma once
#include "Mesh.h"

class Static_Mesh : public Mesh
{
public:
	Static_Mesh();
	void CreateMesh(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices);
	void CreateBillboard(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices);
	void CreateInstancedBillboard(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices);
	void CreateMeshNorm(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices);

	void UpdateInstancedBillboard(GLfloat position_size_data[], GLubyte color_data[], int ParticlesCount);

	void RenderMesh();
	void RenderTessellatedMesh();
	void RenderInstancedBillboard();
	void RenderQuad();
	void RenderCube();
	~Static_Mesh();
private:
	void ClearMesh();
	GLuint particles_position_buffer = 0;
	GLuint particles_color_buffer = 0;
	int particlesCount = 0;
};

