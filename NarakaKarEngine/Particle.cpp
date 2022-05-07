#include "Particle.h"

Particle::Particle() : pos{ glm::vec3(0.0f, 0.0f, 0.0f) }, speed{ glm::vec3(0.0f, 0.0f, 0.0f) },
						r{ 0 }, g{ 0 }, b{ 0 }, a{1},
						size{ 0.0f }, angle{0.0f}, weight{0.0f}
{
}

bool Particle::operator<(const Particle& that) const
{
	// Sort in reverse order : far particles drawn first.
	return this->cameradistance > that.cameradistance;
}

Particle::~Particle()
{
}
