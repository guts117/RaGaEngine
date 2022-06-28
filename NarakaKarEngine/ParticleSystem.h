#ifndef PARTICLESYSTEM
#define PARTICLESYSTEM

#include "Billboard_Mesh.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Particle;

		class ParticleSystem :
			public Billboard_Mesh
		{
		public:
			explicit ParticleSystem();

			ParticleSystem(ParticleSystem&& rhs) noexcept = default;
			ParticleSystem& operator= (ParticleSystem&& rhs) noexcept = default;

			ParticleSystem(const ParticleSystem& rhs) = delete;
			ParticleSystem& operator= (const ParticleSystem& rhs) = delete;

			void UpdateParticlesMeshCPU();
			void GenerateParticlesCPU(GLfloat delta, glm::vec3 Pos);
			void SimulateParticlesCPU(glm::vec3 CameraPosition, GLfloat delta);

			~ParticleSystem() = default;
		private:
			int FindUnusedParticles();
			void SortParticles();

			std::unique_ptr<GLfloat> particles_pos_size;
			std::unique_ptr<GLubyte> particles_color;
			std::unique_ptr<Particle> ParticlesContainer;
			GLuint LastUsedParticle = 0;
		};
	}
}
#endif