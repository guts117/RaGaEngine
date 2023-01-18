#include "engine_ui_pch.h"
#include "EngineUIMain.h"
#include "SceneViewer.h"
#include "Window.h"

using namespace NarakaKarEngine;
using namespace EngineUI;

extern int ScreenWidth;
extern int ScreenHeight;
extern bool IsUpdateFrameBuffersSize;

struct EngineUIMain::Impl
{
	int hideWindowIndex;
	std::unique_ptr<Window> mainWindow;
	std::unique_ptr<std::vector<std::shared_ptr<SceneViewer>>> m_sceneList;

	Impl() = delete;

	Impl(const bool installCallbacks, const std::string version)
		: m_sceneList{ std::make_unique<std::vector<std::shared_ptr<SceneViewer>>>() }
		, mainWindow{ std::make_unique<Window>() }
	{		
		mainWindow->Initialise();
		//Initialize IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(mainWindow->GetWindow(), installCallbacks);
		ImGui_ImplOpenGL3_Init(version.c_str());
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
	}

	Impl(Impl&& rhs) = delete;
	Impl& operator=(Impl&& rhs) = delete;

	Impl(const Impl& rhs) = delete;
	Impl& operator=(const Impl& rhs) = delete;

	void Update(const glm::ivec2& screenDims)
	{
		//get + handle user input events
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
;
		auto isHoveredCnt = 0; 

		for (int i = 0; i < m_sceneList->size(); ++i)
		{
			ImGui::Begin(m_sceneList->at(i)->GetViewerName().c_str(), nullptr, ImGuiWindowFlags_NoFocusOnAppearing);
			ImGui::SetWindowSize(ImVec2(screenDims.x / 2, screenDims.y / 2));
			ImGui::Image((ImTextureID)m_sceneList->at(i)->GetTextureId(), ImGui::GetWindowSize(), ImVec2(0, 1), ImVec2(1, 0));

			auto isSelected = false;
			auto isDragging = ImGui::IsMouseDragging(ImGuiMouseButton_Left);

			if (!isDragging)
			{
				auto pos = ImGui::GetWindowPos();
				auto maxx = pos.x + ImGui::GetWindowWidth();
				auto maxy = pos.y + ImGui::GetWindowHeight();
				auto isHovered = ImGui::IsMouseHoveringRect(pos, ImVec2(maxx, maxy));
				auto isFocused = ImGui::IsWindowDocked() ? ImGui::IsWindowFocused() : true;

				if (m_sceneList->at(i)->GetViewerType() == Editor && hideWindowIndex < 0)
				{
					isSelected = isHovered && isFocused;
					if (isSelected)
					{
						++isHoveredCnt;
						if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) { hideWindowIndex = i; }
						ImGui::SetWindowFocus(m_sceneList->at(i)->GetViewerName().c_str());
						ImGui::GetIO().WantCaptureMouse = false;
					}
				}
				else if(m_sceneList->at(i)->GetViewerType() == InGame && hideWindowIndex == i || hideWindowIndex < 0)
				{
					isSelected = isHovered && isFocused;
					if (isSelected) 
					{
						++isHoveredCnt;
						hideWindowIndex = i;
						ImGui::SetWindowFocus(m_sceneList->at(i)->GetViewerName().c_str());
						ImGui::GetIO().WantCaptureMouse = false;
					}
				}
			}

			m_sceneList->at(i)->InvokeSelectCallback(isSelected);

			ImGui::End();
		}

		if (isHoveredCnt == 0 && !ImGui::GetIO().WantCaptureMouse)
		{
			ImGui::GetIO().WantCaptureMouse = true;
			ImGui::SetWindowFocus(nullptr);
			hideWindowIndex = -1;
		}

		if(hideWindowIndex >= 0)
		{
			mainWindow->SetCursorActive(false);
			ImGui::GetIO().ConfigFlags |= (ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NoMouseCursorChange);
		}
		else
		{
			mainWindow->SetCursorActive(true);
			ImGui::GetIO().ConfigFlags &= ~(ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NoMouseCursorChange);
		}

		ImGui::Begin("Rendering Graph");
		ImGui::Text("ToDo: Show Shader Hierarchy Here!!!!");
		ImGui::SetWindowSize(ImVec2(screenDims.x / 2, screenDims.y / 2));
		ImGui::End();
	}

	void EndUpdate()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		mainWindow->swapBuffers();

		IsUpdateFrameBuffersSize = false;
	}

	void AddSceneViewers(GLuint sceneTex, std::string sceneName, SceneViewerType viewerType, std::function<void(bool)> selectCallback)
	{
		auto scene = std::make_shared<SceneViewer>(sceneTex, sceneName, viewerType, selectCallback);
		m_sceneList->push_back(scene);
	}

	~Impl()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
};

EngineUIMain::EngineUIMain(const bool installCallbacks, const std::string version)
	: m_pImpl{ new Impl(installCallbacks, version) }
{
}

void EngineUIMain::CreateInputCallbacks(const GLFWkeyfun& keyCallback, const GLFWcursorposfun& cursorPosCallback
	, const GLFWmousebuttonfun& mouseBtnCallback, const GLFWscrollfun& mouseScrollCallback)
{
	Pimpl()->mainWindow->CreateInputCallbacks(keyCallback, cursorPosCallback, mouseBtnCallback, mouseScrollCallback);
}

void EngineUIMain::Update(const glm::ivec2& screenDims)
{
	Pimpl()->Update(screenDims);
}

void EngineUIMain::EndUpdate()
{
	Pimpl()->EndUpdate();
}

void EngineUIMain::AddSceneViewers(GLuint sceneTex, std::string sceneName, SceneViewerType viewerType, std::function<void(bool)> selectCallback)
{
	Pimpl()->AddSceneViewers(sceneTex, sceneName, viewerType, selectCallback);
}

GLFWwindow* EngineUIMain::GetMainWindow()
{
	return Pimpl()->mainWindow->GetWindow();
}

glm::ivec2 EngineUIMain::GetScreenDimensions() 
{
	return glm::ivec2(ScreenWidth, ScreenHeight);
}

bool EngineUIMain::IsUpdateBufferSize()
{
	return IsUpdateFrameBuffersSize;
}

bool EngineUIMain::AddKeyBoardButtonEvent(int key, bool down)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddKeyEvent(static_cast<ImGuiKey>(key), down);

	return !io.WantCaptureKeyboard;
}

bool EngineUIMain::AddCursorPosEvent(float x, float y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMousePosEvent(x, y);

	return !io.WantCaptureMouse;
}

bool EngineUIMain::AddMouseButtonEvent(int mouse_button, bool down)
{
	// (1) ALWAYS forward mouse data to ImGui! This is automatic with default backends. With your own backend:
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseButtonEvent(mouse_button, down);

	// (2) ONLY forward mouse data to your underlying app/game.
	return !io.WantCaptureMouse;
}

bool EngineUIMain::AddMouseScrollEvent(float wheel_x, float wheel_y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseWheelEvent(wheel_x, wheel_y);

	return !io.WantCaptureMouse;
}

bool EngineUIMain::IsEnd()
{
	return Pimpl()->mainWindow->getShouldClose();
}

EngineUIMain::~EngineUIMain() = default;