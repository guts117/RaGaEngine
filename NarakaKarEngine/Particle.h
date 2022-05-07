#pragma once
#include <glm/glm.hpp>
class Particle
{
public:
	Particle();
	glm::vec3 pos, speed;
	unsigned char r, g, b, a; // Color
	float size, angle, weight;
	float life = -1.0f; // Remaining life of the particle. if <0 : dead and unused.
	float cameradistance = -1.0f; // *Squared* distance to the camera. if dead : -1.0f

	bool operator<(const Particle& that) const;

	~Particle();
};

