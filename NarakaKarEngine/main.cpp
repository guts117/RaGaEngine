#include "pch.h"
#include "RenderEngineMain.h"
#include "EngineUICreator.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;
using namespace EngineUI;

int main()
{
	std::unique_ptr<RenderEngineMain> engine = std::make_unique<RenderEngineMain>();
	std::unique_ptr<EngineUICreator> engineUI = std::make_unique<EngineUICreator>(engine->Init(), true, "#version 460");

	do 
	{
		//get + handle user input events
		glfwPollEvents();
		engineUI->Update();
		engine->Update([&]{ engineUI->Render(); });
	} while (!engine->IsEnd());

	return 0;
}