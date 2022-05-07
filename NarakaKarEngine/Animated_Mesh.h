#pragma once
#include "Mesh.h"

class Animated_Mesh : public Mesh
{
public:
	Animated_Mesh();
	void CreateAnimatedMesh(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices, std::vector<VertexBoneData>& Bones);
	void RenderAnimatedMesh();

	~Animated_Mesh();

private:
	void ClearMesh();
};

