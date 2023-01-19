#ifndef PARTICLESYSTEM
#define PARTICLESYSTEM

namespace NarakaRenderEngine
{
	namespace RenderEngine
	{
		class Particle;

		class ParticleSystem
		{
		public:
			explicit ParticleSystem();

			ParticleSystem(ParticleSystem&& rhs) noexcept = default;
			ParticleSystem& operator= (ParticleSystem&& rhs) noexcept = default;

			ParticleSystem(const ParticleSystem& rhs) = delete;
			ParticleSystem& operator= (const ParticleSystem& rhs) = delete;

			void CreateInstancedMesh(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices);
			void UpdateParticlesMeshCPU();
			void GenerateParticlesCPU(GLfloat delta, glm::vec3 Pos);
			void SimulateParticlesCPU(glm::vec3 CameraPosition, GLfloat delta);
			void RenderInstancedMesh();

			~ParticleSystem() = default;
		private:
			int FindUnusedParticles();
			void SortParticles();
			void UpdateInstancedBillboard(GLfloat position_size_data[], GLubyte color_data[], int ParticlesCount);

			std::unique_ptr<GLfloat> particles_pos_size;
			std::unique_ptr<GLubyte> particles_color;
			std::unique_ptr<Particle> ParticlesContainer;
			GLuint LastUsedParticle = 0;
			GLuint indexCount = 0, vertexCount = 0, VAO = 0, VBO = 0, IBO = 0, position_buffer = 0, color_buffer = 0, instanceCount = 0;
		};
	}
}
#endif