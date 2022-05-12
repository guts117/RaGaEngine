#include "ParticleSystem.h"
int ParticleSystem::particlesCount = 0;
int ParticleSystem::LastUsedParticle = 0;

ParticleSystem::ParticleSystem()
{
	particles_pos_size = new GLfloat[MaxParticles * 4]{ 0.0f };
	particles_color = new GLubyte[MaxParticles * 4]{};

	ParticlesContainer = new Particle[MaxParticles];

}

void ParticleSystem::CreateParticlesMeshCPU(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices)
{
	indexCount = numOfIndices;
	vertexCount = numOfVertices;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * numOfIndices, Indices, GL_STATIC_DRAW);


	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices[0]) * numOfVertices, Vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(0);


	// The VBO containing the positions and sizes of the particles
	glGenBuffers(1, &particles_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	// 2nd attribute buffer : positions of particles' centers
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		4,                                // size : x + y + z + size => 4
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);
	glEnableVertexAttribArray(1);

	// The VBO containing the colors of the particles
	glGenBuffers(1, &particles_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
	// 3rd attribute buffer : particles' colors
	glVertexAttribPointer(
		2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		4,                                // size : r + g + b + a => 4
		GL_UNSIGNED_BYTE,                 // type
		GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
		0,                                // stride
		(void*)0                          // array buffer offset
	);
	glEnableVertexAttribArray(2);

	// These functions are specific to glDrawArrays*Instanced*.
	// The first parameter is the attribute buffer we're talking about.
	// The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
	// http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
	glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
	glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
	glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ParticleSystem::UpdateParticlesMeshCPU()
{
	/*particlesCount = 165;*/
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, particlesCount * sizeof(GLfloat) * 4, particles_pos_size);

	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, particlesCount * sizeof(GLubyte) * 4, particles_color);
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
		ParticlesContainer[particleIndex].life = 3.0f; // This particle will live 5 seconds.
		ParticlesContainer[particleIndex].pos = Pos;

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

		ParticlesContainer[particleIndex].speed = maindir + randomdir * spread;


		// Very bad way to generate a random color
		ParticlesContainer[particleIndex].r = rand() % 256;
		ParticlesContainer[particleIndex].g = rand() % 256;
		ParticlesContainer[particleIndex].b = rand() % 256;
		ParticlesContainer[particleIndex].a = (rand() % 256) / 3;

		ParticlesContainer[particleIndex].size = (rand() % 1000) / 20000.0f + 0.1f;

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

		Particle& p = ParticlesContainer[i]; // shortcut

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
				particles_pos_size[4 * ParticlesCount + 0] = p.pos.x;
				particles_pos_size[4 * ParticlesCount + 1] = p.pos.y;
				particles_pos_size[4 * ParticlesCount + 2] = p.pos.z;

				particles_pos_size[4 * ParticlesCount + 3] = p.size;

				particles_color[4 * ParticlesCount + 0] = p.r;
				particles_color[4 * ParticlesCount + 1] = p.g;
				particles_color[4 * ParticlesCount + 2] = p.b;
				particles_color[4 * ParticlesCount + 3] = p.a;

			}
			else {
				// Particles that just died will be put at the end of the buffer in SortParticles();
				p.cameradistance = -1.0f;
			}
			ParticlesCount++;
		}
	}

	SortParticles();
	particlesCount = ParticlesCount;

}

void ParticleSystem::RenderParticlesMeshCPU()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElementsInstanced(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0, particlesCount);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

ParticleSystem::~ParticleSystem()
{
	ClearMesh();
}

int ParticleSystem::FindUnusedParticles()
{
	for (int i = LastUsedParticle; i < MaxParticles; i++) {
		if (ParticlesContainer[i].life < 0) {
			LastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < LastUsedParticle; i++) {
		if (ParticlesContainer[i].life < 0) {
			LastUsedParticle = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one
}

void ParticleSystem::SortParticles()
{
	std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles]);
}

void ParticleSystem::ClearMesh()
{
	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}
	if (VAO != 0) {
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}
	if (IBO != 0) {
		glDeleteBuffers(1, &IBO);
		IBO = 0;
	}

	if (particles_position_buffer != 0) {
		glDeleteBuffers(1, &particles_position_buffer);
		particles_position_buffer = 0;
	}

	if (particles_color_buffer != 0) {
		glDeleteBuffers(1, &particles_color_buffer);
		particles_color_buffer = 0;
	}

	if (particles_pos_size != nullptr) {
		delete[] particles_pos_size;
		particles_pos_size = nullptr;
	}
	if (particles_color != nullptr) {
		delete[] particles_color;
		particles_color = nullptr;
	}
	if (ParticlesContainer != nullptr) {
		delete[] ParticlesContainer;
		ParticlesContainer = nullptr;
	}

	particlesCount = 0;
	indexCount = 0;
	vertexCount = 0;
}
