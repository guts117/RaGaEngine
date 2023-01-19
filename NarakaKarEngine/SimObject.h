#ifndef SIM_OBJECT
#define SIM_OBJECT

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
		Transform* transform;
		NarakaRenderEngine::Render_Object* renderObject;
		
		
	};
}

#endif