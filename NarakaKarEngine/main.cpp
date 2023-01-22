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
	auto physicsEngine = std::make_unique<PhysicsEngineMain>();
	auto creator = EngineEditorMain();
	glm::ivec2 screenDims = glm::ivec2(creator.GetScreenDimensions());
	std::unique_ptr<RenderEngineMain> renderEngine = std::make_unique<RenderEngineMain>(screenDims);
	creator.AddSceneViewers(renderEngine.get());

	do 
	{
		screenDims = glm::ivec2(creator.GetScreenDimensions());
		physicsEngine->Update(0.016f);
		renderEngine->Update(screenDims, creator.IsUpdateBufferSize());
		creator.Update(screenDims);
		renderEngine->EndUpdate();
		creator.EndUpdate();
	} while (!creator.IsEnd());

	return 0;
}