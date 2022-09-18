#include "pch.h"
#include "RenderEngineMain.h"
#include "EngineUIMain.h"
#include "PhysicsEngineMain.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;
using namespace EngineUI;

int main()
{
	std::unique_ptr<RenderEngineMain> engine = std::make_unique<RenderEngineMain>();
	std::unique_ptr<EngineUIMain> engineUI = std::make_unique<EngineUIMain>(engine->Init(), true, "#version 460");
	std::unique_ptr<PhysicsEngineMain> physicsEngine = std::make_unique<PhysicsEngineMain>();

	do 
	{
		//get + handle user input events
		glfwPollEvents();
		physicsEngine->Update(0.0f);
		engineUI->Update();
		engine->Update();
		engineUI->EndUpdate();
		engine->EndUpdate();
	} while (!engine->IsEnd());

	return 0;
}