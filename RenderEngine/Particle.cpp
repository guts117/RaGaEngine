#include "render_pch.h"
#include "Particle.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

bool Particle::operator<(const Particle& that) const
{
	// Sort in reverse order : far particles drawn first.
	return this->cameradistance > that.cameradistance;
}