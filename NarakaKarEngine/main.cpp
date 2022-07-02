#include "pch.h"
#include "RenderEngineMain.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

int main()
{
	std::unique_ptr<RenderEngineMain> engine = std::make_unique<RenderEngineMain>();
	
	engine->Init();

	do 
	{
		engine->Update();
	} while (!engine->IsEnd());

	return 0;
}