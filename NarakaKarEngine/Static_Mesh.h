#ifndef STATIC_MESH
#define STATIC_MESH

#include "Mesh.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Static_Mesh : public Mesh
		{
		public:
			explicit Static_Mesh() = default;

			Static_Mesh(Static_Mesh&& rhs) noexcept = default;
			Static_Mesh& operator= (Static_Mesh&& rhs) noexcept = default;

			Static_Mesh(const Static_Mesh& rhs) = delete;
			Static_Mesh& operator= (const Static_Mesh& rhs) = delete;

			void CreateMeshWithNormal(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices) override;
			void CreateMeshWithTangentNormal(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices) override;

			void RenderMesh() override;
			void RenderTessellatedMesh() override;
			void RenderQuad();
			void RenderCube();

			void RenderVolumeCube();

			~Static_Mesh() = default;
		};
	}
}
#endif