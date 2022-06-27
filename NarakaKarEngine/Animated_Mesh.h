#ifndef ANIMATED_MESH
#define ANIMATED_MESH

#include "Mesh.h"

class VertexBoneData;

class Animated_Mesh : public Mesh
{
public:
	explicit Animated_Mesh() = default;

	Animated_Mesh(Animated_Mesh&& rhs) noexcept = default;
	Animated_Mesh& operator= (Animated_Mesh&& rhs) noexcept = default;

	Animated_Mesh(const Animated_Mesh& rhs) = delete;
	Animated_Mesh& operator= (const Animated_Mesh& rhs) = delete;

	void CreateMeshWithBones(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices, std::vector<VertexBoneData>& Bones) override;
	void RenderMesh() override;

	~Animated_Mesh();

private:
	GLuint VBO_bones = 0;
	std::vector<VertexBoneData> Bones;

};
#endif
