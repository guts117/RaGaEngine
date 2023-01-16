#include "pch.h"
#include "EngineEditor.h"
#include "EngineUIMain.h"
#include "RenderEngineMain.h"

//std::vector<std::function<void(bool)>> RenderEngineMain::AddViewers(EngineUIMain* engineUI)
//{
//	engineUI->AddSceneViewers(Pimpl()->exposureFbo->GetFBOBuffer(0, 0), "EditorView", Editor, [this](bool isSelected) { Pimpl()->isEditorViewSelected = isSelected; });
//	engineUI->AddSceneViewers(Pimpl()->cameraBlitFbo->GetFBOBuffer(0, 0), "GameView", InGame, [this](bool isSelected) { Pimpl()->isGameViewSelected = isSelected; });
//}