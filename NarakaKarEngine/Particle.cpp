#include "pch.h"
#include "Particle.h"

bool Particle::operator<(const Particle& that) const
{
	// Sort in reverse order : far particles drawn first.
	return this->cameradistance > that.cameradistance;
}