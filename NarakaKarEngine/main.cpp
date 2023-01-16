#include "pch.h"
#include "RenderEngineMain.h"
#include "EngineUIMain.h"
#include "PhysicsEngineMain.h"
#include "EngineInputManager.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;
using namespace EngineUI;
using namespace PhysicsEngine;
using namespace InputManager;

std::unique_ptr<PhysicsEngineMain> physicsEngine;
std::unique_ptr<EngineUIMain> engineUI;

int main()
{
	auto inputmanager = EngineInputManager();
	physicsEngine = std::make_unique<PhysicsEngineMain>();
	engineUI = std::make_unique<EngineUIMain>(true, "#version 460");
	engineUI->CreateInputCallbacks(EngineInputManager::HandleKeysPresses, EngineInputManager::HandleCursorPosition, EngineInputManager::HandleMousePresses, EngineInputManager::HandleMouseScrolls);
	glm::ivec2 screenDims = glm::ivec2(engineUI->GetScreenDimensions());
	std::unique_ptr<RenderEngineMain> renderEngine = std::make_unique<RenderEngineMain>(screenDims);

	do 
	{
		screenDims = glm::ivec2(engineUI->GetScreenDimensions());
		//get + handle user input events
		glfwPollEvents();
		physicsEngine->Update(0.016f);
		renderEngine->Update(screenDims, engineUI->IsUpdateBufferSize());
		engineUI->Update(screenDims);
		engineUI->EndUpdate();
		renderEngine->EndUpdate();
	} while (!engineUI->IsEnd());

	return 0;
}