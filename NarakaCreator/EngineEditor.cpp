#include "creator_pch.h"
#include "EngineEditor.h"
#include "EngineUIMain.h"
#include "RenderEngineMain.h"
#include "SceneViewer.h"
#include "EngineInputManager.h"

using namespace NarakaKarEngine;
using namespace NarakaCreator;
using namespace RenderEngine;
using namespace NarakaCreator;
using namespace EngineUI;
using namespace InputManager;

struct EngineEditor::Impl
{
	std::vector<RendererToViewer> m_renderer2Viewers;
	bool isEditorViewSelected;
	bool isGameViewSelected;
		
	Impl() = delete;

	Impl(std::vector<RendererToViewer>&& render2Views)
		: m_renderer2Viewers{ std::move(render2Views) }
		, isEditorViewSelected { false }
		, isGameViewSelected { false }
	{
	}

	Impl(Impl&& rhs) = delete;
	Impl& operator=(Impl&& rhs) = delete;

	Impl(const Impl& rhs) = delete;
	Impl& operator=(const Impl& rhs) = delete;

	void Update()
	{
		if (isEditorViewSelected)
		{
			//cameras[1]->keyControl(mainWindow->getKeys(), deltaTime);
			//auto xChange = 0.0f;
			//auto yChange = 0.0f;

			if (EngineInputManager::IsLeftMousePress())
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
	}

	void AddSceneViewers(EngineUIMain* engineUI, const RenderEngineMain* renderEngineMain)
	{
		auto editorSelectCallback = [this](bool isSelected) { isEditorViewSelected = isSelected; };
		auto gameSelectCallback = [this](bool isSelected) { isGameViewSelected = isSelected; };
		for(auto r2v : m_renderer2Viewers)
		{
			auto fbo = renderEngineMain->GetFboBuffer(r2v.sceneFboName, r2v.fboIndex, r2v.bufferIndex);
			
			std::function<void(bool)> selectCallback;
			if (r2v.viewerType == Editor)	{ selectCallback = editorSelectCallback; }
			else							{ selectCallback = gameSelectCallback; }
			engineUI->AddSceneViewers(fbo, r2v.viewerName, static_cast<SceneViewerType>(r2v.viewerType), selectCallback);
		}
	}

	~Impl() = default;
};

EngineEditor::EngineEditor(std::vector<RendererToViewer>&& render2Views)
	: m_pImpl{ std::make_unique<Impl>(std::move(render2Views))}
{
}

void EngineEditor::Update()
{
	Pimpl()->Update();
}

void EngineEditor::EndUpdate()
{
	Pimpl()->EndUpdate();
}

void EngineEditor::AddSceneViewers(EngineUIMain* engineUI, const RenderEngineMain* renderEngineMain)
{
	Pimpl()->AddSceneViewers(engineUI, renderEngineMain);
}

EngineEditor::~EngineEditor() = default;
