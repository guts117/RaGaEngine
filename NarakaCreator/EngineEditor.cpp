#include "creator_pch.h"
#include "EngineEditor.h"
#include "EngineUIMain.h"
#include "RenderEngineMain.h"
#include "SceneViewer.h"
#include "EngineInputManager.h"

using namespace NarakaCreator;
using namespace NarakaRenderEngine;
using namespace RenderEngine;
using namespace EngineUI;
using namespace InputManager;

struct EngineEditor::Impl
{
	EngineUIMain engineUI;
	std::vector<RendererToViewer> m_renderer2Viewers;
	bool isEditorViewSelected;
	bool isGameViewSelected;
		
	Impl()
		: engineUI { true, "#version 460" }
		, m_renderer2Viewers{ RendererToViewer { "Final_Output_Pass", "EditorView", Editor, 0, 0 } , RendererToViewer { "CameraPass0", "GameView", InGame, 0, 0 } }
		, isEditorViewSelected { false }
		, isGameViewSelected { false }
	{
	}

	Impl(Impl&& rhs) = delete;
	Impl& operator=(Impl&& rhs) = delete;

	Impl(const Impl& rhs) = delete;
	Impl& operator=(const Impl& rhs) = delete;

	void Update(const glm::ivec2& screenDims)
	{
		engineUI.Update(screenDims);

		if (isEditorViewSelected)
		{
			//cameras[1]->keyControl(mainWindow->getKeys(), deltaTime);
			//auto xChange = 0.0f;
			//auto yChange = 0.0f;

			if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
			{
				std::cout << "yes editor" << std::endl;
			//	xChange = mainWindow->getXChange();
			//	yChange = mainWindow->getYChange();
			}
			//cameras[1]->mouseControl(xChange, yChange, mainWindow->scrollVal, deltaTime);

		}
		else if(isGameViewSelected)
		{
			if (EngineInputManager::GetKeys()[GLFW_KEY_L])
			{
				std::cout << "L" << std::endl;
				//spotLights[0]->Toggle();
				//mainWindow->getKeys()[GLFW_KEY_L] = false;
			}
			//std::cout << "yes game" << std::endl;
			//cameras[0]->keyControl(mainWindow->getKeys(), deltaTime);
			//cameras[0]->mouseControl(mainWindow->getXChange(), mainWindow->getYChange(), 0, 0);
		}
	}

	void EndUpdate()
	{
		engineUI.EndUpdate();
	}

	void AddSceneViewers(const RenderEngineMain* renderEngineMain)
	{
		auto editorSelectCallback = [this](bool isSelected) { isEditorViewSelected = isSelected; };
		auto gameSelectCallback = [this](bool isSelected) { isGameViewSelected = isSelected; };
		for(auto r2v : m_renderer2Viewers)
		{
			auto fbo = renderEngineMain->GetFboBuffer(r2v.sceneFboName, r2v.fboIndex, r2v.bufferIndex);
			
			std::function<void(bool)> selectCallback;
			if (r2v.viewerType == Editor)	{ selectCallback = editorSelectCallback; }
			else							{ selectCallback = gameSelectCallback; }
			engineUI.AddSceneViewers(fbo, r2v.viewerName, static_cast<SceneViewerType>(r2v.viewerType), selectCallback);
		}
	}

	~Impl() = default;
};

EngineEditor::EngineEditor()
	: m_pImpl{ std::make_unique<Impl>()}
{
}

bool EngineEditor::IsUpdateBufferSize() 
{
	return Pimpl()->engineUI.IsUpdateBufferSize();
}

glm::ivec2 EngineEditor::GetScreenDimensions()
{
	return Pimpl()->engineUI.GetScreenDimensions();
}

bool EngineEditor::IsEnd()
{
	return Pimpl()->engineUI.IsEnd();
}
void EngineEditor::Update(const glm::ivec2& screenDims)
{
	Pimpl()->Update(screenDims);
}

void EngineEditor::EndUpdate()
{
	Pimpl()->EndUpdate();
}

void EngineEditor::AddSceneViewers(const RenderEngineMain* renderEngineMain)
{
	Pimpl()->AddSceneViewers(renderEngineMain);
}

EngineEditor::~EngineEditor() = default;
