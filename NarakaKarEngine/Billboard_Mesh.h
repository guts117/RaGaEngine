#ifndef BILLBOARD_MESH
#define BILLBOARD_MESH

namespace NarakaKarEngine
{
    namespace RenderEngine
    {
        //ToDo: Incorporate all of this in Mesh class as well
        class Billboard_Mesh
        {
        public:
            explicit Billboard_Mesh() = default;

            Billboard_Mesh(Billboard_Mesh&& rhs) noexcept = default;
            Billboard_Mesh& operator= (Billboard_Mesh&& rhs) noexcept = default;

            Billboard_Mesh(const Billboard_Mesh& rhs) = delete;
            Billboard_Mesh& operator= (const Billboard_Mesh& rhs) = delete;

            void CreateMesh(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices);
            void CreateInstancedMesh(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices);
            void RenderMesh();
            void RenderInstancedMesh();

            void UpdateInstancedBillboard(GLfloat position_size_data[], GLubyte color_data[], int ParticlesCount);

            ~Billboard_Mesh();

        protected:
            GLuint indexCount = 0, vertexCount = 0, VAO = 0, VBO = 0, IBO = 0, position_buffer = 0, color_buffer = 0, instanceCount = 0;
        };
    }
}
#endif