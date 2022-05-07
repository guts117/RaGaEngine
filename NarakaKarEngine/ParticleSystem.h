#pragma once
#include "Mesh.h"
#include "Particle.h"
#include <glm/gtx/norm.hpp>
#include <iostream>

class ParticleSystem :
	public Mesh
{
public:
	ParticleSystem();

	void CreateParticlesMeshCPU(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices);
	void UpdateParticlesMeshCPU();
	void GenerateParticlesCPU(GLfloat delta, glm::vec3 Pos);
	void SimulateParticlesCPU(glm::vec3 CameraPosition, GLfloat delta);
	
	void RenderParticlesMeshCPU();

	~ParticleSystem();
private:
	int FindUnusedParticles();
	void SortParticles();

	void ClearMesh();

	GLfloat* particles_pos_size;
	GLubyte* particles_color;

	GLuint particles_position_buffer = 0;
	GLuint particles_color_buffer = 0;
	static int particlesCount;

	static int LastUsedParticle;

	Particle* ParticlesContainer;
};

