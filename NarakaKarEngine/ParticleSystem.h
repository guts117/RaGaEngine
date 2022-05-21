#ifndef PARTICLESYSTEM
#define PARTICLESYSTEM

#include "Mesh.h"
#include "Particle.h"
#include <glm/gtx/norm.hpp>
#include <iostream>

class ParticleSystem :
	public Mesh
{
public:
	explicit ParticleSystem();

	ParticleSystem(ParticleSystem&& rhs) noexcept = default;
	ParticleSystem& operator= (ParticleSystem&& rhs) noexcept = default;

	ParticleSystem(const ParticleSystem& rhs) = delete;
	ParticleSystem& operator= (const ParticleSystem& rhs) = delete;

	void CreateInstancedMesh(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices) override;
	void RenderMesh() override;

	void UpdateParticlesMeshCPU();
	void GenerateParticlesCPU(GLfloat delta, glm::vec3 Pos);
	void SimulateParticlesCPU(glm::vec3 CameraPosition, GLfloat delta);

	~ParticleSystem();
private:
	int FindUnusedParticles();
	void SortParticles();

	std::unique_ptr<GLfloat> particles_pos_size;
	std::unique_ptr<GLubyte> particles_color;
	std::unique_ptr<Particle> ParticlesContainer;
	GLuint particles_position_buffer = 0, particles_color_buffer = 0, particlesCount = 0, LastUsedParticle = 0;
};

#endif