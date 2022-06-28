#ifndef PARTICLE
#define PARTICLE

#include "pch.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Particle
		{
		public:
			explicit Particle() = default;

			glm::vec3 pos{ glm::vec3(0.0f, 0.0f, 0.0f) }, speed{ glm::vec3(0.0f, 0.0f, 0.0f) };
			unsigned char r{ 0 }, g{ 0 }, b{ 0 }, a{ 1 };					// Color
			float size{ 0.0f }, angle{ 0.0f }, weight{ 0.0f };
			float life{ -1.0f };											// Remaining life of the particle. if <0 : dead and unused.
			float cameradistance{ -1.0f };									// *Squared* distance to the camera. if dead : -1.0f

			bool operator<(const Particle& that) const;

			~Particle() = default;
		};
	}
}
#endif