#ifndef SIM_OBJECT
#define SIM_OBJECT

#include "pch.h"

namespace NarakaPhysicsEngine
{
	class PhysicsEngine;
}
namespace NarakaRenderEngine
{
	class Render_Object;
}

namespace NarakaKarEngine
{
	struct Transform;
	struct SimObject
	{
		std::string name = "";
		unsigned long uniqueSimId = 0;													//Use this for object identification and sorting; 
		Transform* transform = nullptr;													//ptr to pool
		NarakaRenderEngine::Render_Object* renderObject = nullptr;						//ptr to pool
		std::weak_ptr<SimObject> parentNode = std::weak_ptr<SimObject>();				//ptr to parent node
	};
}

#endif