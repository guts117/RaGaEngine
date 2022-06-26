#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
	: Billboard_Mesh()
	, particles_pos_size{ new GLfloat[MaxParticles * 4]{ 0.0f } }
	, particles_color{ new GLubyte[MaxParticles * 4]{} }
	, ParticlesContainer{ new Particle[MaxParticles] }
{}

void ParticleSystem::UpdateParticlesMeshCPU()
{
	UpdateInstancedBillboard(particles_pos_size.get(), particles_color.get(), instanceCount);
}

void ParticleSystem::GenerateParticlesCPU(GLfloat delta, glm::vec3 Pos)
{
	/*particlesCount = 165;
	for (int i = 0; i < 165; i += 4) {
		particles_pos_size[i] = 10.0f;
		particles_pos_size[i + 1] = 3.0f + i;
		particles_pos_size[i + 3] = 0.1f;
	}*/

	// Generate 10 new particule each millisecond,
		// but limit this to 16 ms (60 fps), or if you have 1 long frame (1sec),
		// newparticles will be huge and the next frame even longer.
	int newparticles = (int)(delta * 100000.0);
		if (newparticles > (int)(0.016f * 100000.0))
			newparticles = (int)(0.016f * 100000.0);
	for (int i = 0; i < newparticles; i++) {
		int particleIndex = FindUnusedParticles();
		ParticlesContainer.get()[particleIndex].life = 3.0f; // This particle will live 5 seconds.
		ParticlesContainer.get()[particleIndex].pos = Pos;

		float spread = 1.5f;
		glm::vec3 maindir = glm::vec3(0.0f, 10.0f, 0.0f);
		// Very bad way to generate a random direction; 
		// See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
		// combined with some user-controlled parameters (main direction, spread, etc)
		glm::vec3 randomdir = glm::vec3(
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f
		);

		ParticlesContainer.get()[particleIndex].speed = maindir + randomdir * spread;

		// Very bad way to generate a random color
		ParticlesContainer.get()[particleIndex].r = rand() % 256;
		ParticlesContainer.get()[particleIndex].g = rand() % 256;
		ParticlesContainer.get()[particleIndex].b = rand() % 256;
		ParticlesContainer.get()[particleIndex].a = (rand() % 256) / 3;

		ParticlesContainer.get()[particleIndex].size = (rand() % 1000) / 20000.0f + 0.1f;
	}
}

void ParticleSystem::SimulateParticlesCPU(glm::vec3 CameraPosition, GLfloat delta)
{
	/*std::cout << delta <<std::endl;
	float x = (rand() % 2000 - 1000.0f) / 10000.0f;

	for (int i = 0; i < 165; i += 4) {
		particles_pos_size[i] += x;
	}


	for (int i = 0; i < 165; i += 5) {
		particles_color[i] = rand() % 256;
	}
	for (int j = 3; j < 165; j += 4) {
		particles_color[j] = rand() % 256 /3;
	}*/

	 //Simulate all particles

	int ParticlesCount = 0;
	for (int i = 0; i < MaxParticles; i++) {

		Particle& p = ParticlesContainer.get()[i]; // shortcut

		if (p.life > 0.0f) {

			// Decrease life
			p.life -= delta;
			if (p.life > 0.0f) {

				// Simulate simple physics : gravity only, no collisions
				p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * (float)delta * 0.5f;
				p.pos += p.speed * (float)delta;
				p.cameradistance = glm::length2(p.pos - CameraPosition);
				//ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

				// Fill the GPU buffer
				particles_pos_size.get()[4 * ParticlesCount + 0] = p.pos.x;
				particles_pos_size.get()[4 * ParticlesCount + 1] = p.pos.y;
				particles_pos_size.get()[4 * ParticlesCount + 2] = p.pos.z;

				particles_pos_size.get()[4 * ParticlesCount + 3] = p.size;

				particles_color.get()[4 * ParticlesCount + 0] = p.r;
				particles_color.get()[4 * ParticlesCount + 1] = p.g;
				particles_color.get()[4 * ParticlesCount + 2] = p.b;
				particles_color.get()[4 * ParticlesCount + 3] = p.a;

			}
			else {
				// Particles that just died will be put at the end of the buffer in SortParticles();
				p.cameradistance = -1.0f;
			}
			ParticlesCount++;
		}
	}

	SortParticles();
	instanceCount = ParticlesCount;
}

int ParticleSystem::FindUnusedParticles()
{
	for (int i = LastUsedParticle; i < MaxParticles; i++) {
		if (ParticlesContainer.get()[i].life < 0) {
			LastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < LastUsedParticle; i++) {
		if (ParticlesContainer.get()[i].life < 0) {
			LastUsedParticle = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one
}

void ParticleSystem::SortParticles()
{
	std::sort(&ParticlesContainer.get()[0], &ParticlesContainer.get()[MaxParticles]);
}