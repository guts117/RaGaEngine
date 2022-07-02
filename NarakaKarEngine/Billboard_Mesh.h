#ifndef BILLBOARD_MESH
#define BILLBOARD_MESH

#include "Mesh.h"

namespace NarakaKarEngine
{
    namespace RenderEngine
    {
        class Billboard_Mesh :
            public Mesh
        {
        public:
            explicit Billboard_Mesh() = default;

            Billboard_Mesh(Billboard_Mesh&& rhs) noexcept = default;
            Billboard_Mesh& operator= (Billboard_Mesh&& rhs) noexcept = default;

            Billboard_Mesh(const Billboard_Mesh& rhs) = delete;
            Billboard_Mesh& operator= (const Billboard_Mesh& rhs) = delete;

            void CreateMesh(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices) override;
            void CreateInstancedMesh(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices) override;
            void RenderMesh() override;
            void RenderInstancedMesh() override;

            void UpdateInstancedBillboard(GLfloat position_size_data[], GLubyte color_data[], int ParticlesCount);

            ~Billboard_Mesh();

        protected:
            GLuint position_buffer = 0, color_buffer = 0, instanceCount = 0;
        };
    }
}
#endif