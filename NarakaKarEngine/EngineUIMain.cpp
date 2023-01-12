#include "pch.h"
#include "EngineUIMain.h"
#include "SceneViewer.h"

using namespace NarakaKarEngine;
using namespace EngineUI;

extern int ScreenWidth;
extern int ScreenHeight;

EngineUIMain::EngineUIMain() = default;

EngineUIMain::EngineUIMain(GLFWwindow* window, const bool installCallbacks, const std::string version)
	: m_sceneList{std::make_unique<std::vector<std::shared_ptr<SceneViewer>>>()}
{
	//Initialize IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, installCallbacks);
	ImGui_ImplOpenGL3_Init(version.c_str());
}

void EngineUIMain::Update(const bool& isMouseHidden)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	auto isFocusedCnt = 0;
	auto isHoveredCnt = 0;
	auto isNoMouse = (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_NoMouse) != 0;

	for (int i = 0; i < m_sceneList->size(); ++i)
	{
		ImGui::Begin(m_sceneList->at(i)->GetViewerName().c_str(), nullptr, ImGuiWindowFlags_NoFocusOnAppearing);
		ImGui::SetWindowSize(ImVec2(ScreenWidth / 2, ScreenHeight / 2));
		ImGui::Image((ImTextureID)m_sceneList->at(i)->GetTextureId(), ImGui::GetWindowSize(), ImVec2(0, 1), ImVec2(1, 0));
		m_sceneList->at(i)->InvokeSelectCallback([&]() -> bool
			{
				auto isHovered = false;		
				auto isFocused = ImGui::IsWindowFocused();
				auto isDragging = ImGui::IsMouseDragging(0);

				if (isFocused)
				{
					++isFocusedCnt;

					if (!isNoMouse)
					{
						ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NoMouseCursorChange;
					}
				}

				if(m_sceneList->at(i)->GetViewerType() == InGame)
				{
					return isFocused && !isDragging;
				}
				else
				{
					if (!isNoMouse)
					{
						isHovered = ImGui::IsWindowHovered();
					}
					else
					{
						auto pos = ImGui::GetWindowPos();
						auto maxx = pos.x + ImGui::GetWindowWidth();
						auto maxy = pos.y + ImGui::GetWindowHeight();
						isHovered = ImGui::IsMouseHoveringRect(pos, ImVec2(maxx, maxy));
					}

					if (isHovered) { ++isHoveredCnt; }

					return isFocused && isHovered && !isDragging;
				}
			}());

		ImGui::End();
	}

	if (isFocusedCnt == 0 && isNoMouse)
	{
		ImGui::GetIO().ConfigFlags &= ~(ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NoMouseCursorChange);
	}

	if(isHoveredCnt == 0 && !isMouseHidden)
	{
		ImGui::SetWindowFocus(nullptr);
	}

	//std::cout << isMouseHidden << std::endl;
	//std::cout << isNoMouse << std::endl;

	ImGui::Begin("Hierarchy");
	ImGui::Text("Put Scene Hierarchy Here!!!!");
	ImGui::SetWindowSize(ImVec2(ScreenWidth / 2, ScreenHeight / 2));
	ImGui::End();
}

void EngineUIMain::EndUpdate()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EngineUIMain::AddSceneViewers(GLuint sceneTex, std::string sceneName, SceneViewerType viewerType, std::function<void(bool)> selectCallback)
{
	auto scene = std::make_shared<SceneViewer>(sceneTex, sceneName, viewerType, selectCallback);
	m_sceneList->push_back(scene);
}

EngineUIMain::~EngineUIMain() 
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
};
