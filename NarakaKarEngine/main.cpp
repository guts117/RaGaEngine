#include "pch.h"
#include "RenderEngineMain.h"
#include "EngineUIMain.h"
#include "PhysicsEngineMain.h"
#include "EngineInputManager.h"
#include "EngineEditor.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;
using namespace EngineUI;
using namespace PhysicsEngine;
using namespace InputManager;
using namespace NarakaEditor;


int main()
{
	auto physicsEngine = std::make_unique<PhysicsEngineMain>();
	auto engineUI = std::make_unique<EngineUIMain>(true, "#version 460");
	engineUI->CreateInputCallbacks(EngineInputManager::HandleKeysPresses, EngineInputManager::HandleCursorPosition, EngineInputManager::HandleMousePresses, EngineInputManager::HandleMouseScrolls);
	glm::ivec2 screenDims = glm::ivec2(engineUI->GetScreenDimensions());
	std::unique_ptr<RenderEngineMain> renderEngine = std::make_unique<RenderEngineMain>(screenDims);

	std::vector<RendererToViewer> sceneViewer = { RendererToViewer { "Final_Output_Pass", "EditorView", Editor, 0, 0 } , RendererToViewer { "CameraPass0", "GameView", InGame, 0, 0 } };
	EngineEditor editor = EngineEditor(std::move(sceneViewer));
	editor.AddSceneViewers(engineUI.get(), renderEngine.get());

	do 
	{
		screenDims = glm::ivec2(engineUI->GetScreenDimensions());
		physicsEngine->Update(0.016f);
		renderEngine->Update(screenDims, engineUI->IsUpdateBufferSize());
		engineUI->Update(screenDims);
		editor.Update();
		renderEngine->EndUpdate();
		engineUI->EndUpdate();
	} while (!engineUI->IsEnd());

	return 0;
}