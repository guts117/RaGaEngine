#include "pch.h"
#include "RenderEngineMain.h"
#include "EngineUIMain.h"
#include "PhysicsEngineMain.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;
using namespace EngineUI;

std::unique_ptr<PhysicsEngineMain> physicsEngine;
std::unique_ptr<EngineUIMain> engineUI;

int main()
{
	physicsEngine = std::make_unique<PhysicsEngineMain>();
	std::unique_ptr<RenderEngineMain> renderEngine = std::make_unique<RenderEngineMain>();
	engineUI = std::make_unique<EngineUIMain>(renderEngine->GetMainWindow(), true, "#version 460");
	renderEngine->AddViewers();

	do 
	{
		//get + handle user input events
		glfwPollEvents();
		physicsEngine->Update(0.016f);
		renderEngine->Update();
		engineUI->Update();
		engineUI->EndUpdate();
		renderEngine->EndUpdate();
	} while (!renderEngine->IsEnd());

	return 0;
}