#include "pch.h"
#include "RenderEngineMain.h"
#include "PhysicsEngineMain.h"
#include "EngineEditorMain.h"

//using namespace NarakaKarEngine;
using namespace NarakaCreator;
using namespace NarakaRenderEngine;
using namespace RenderEngine;
using namespace NarakaPhysicsEngine;
using namespace PhysicsEngine;


int main()
{
	auto physicsEngine = PhysicsEngineMain();
	auto creator = EngineEditorMain();
	glm::ivec2 screenDims = glm::ivec2(creator.GetScreenDimensions());
	auto renderEngine = RenderEngineMain(screenDims);
	creator.AddSceneViewers(&renderEngine);

	do 
	{
		screenDims = glm::ivec2(creator.GetScreenDimensions());
		physicsEngine.Update(0.016f);
		renderEngine.Update(screenDims, creator.IsUpdateBufferSize());
		creator.Update(screenDims);
		renderEngine.EndUpdate();
		creator.EndUpdate();
	} while (!creator.IsEnd());

	return 0;
}