#include "render_pch.h"
#include "EngineUIMain.h"
#include "SceneViewer.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

using namespace NarakaKarEngine;
using namespace EngineUI;

extern int ScreenWidth;
extern int ScreenHeight;

struct EngineUIMain::Impl
{
	std::unique_ptr<std::vector<std::shared_ptr<SceneViewer>>> m_sceneList;

	Impl() = delete;

	Impl(GLFWwindow* window, const bool installCallbacks, const std::string version)
		: m_sceneList{ std::make_unique<std::vector<std::shared_ptr<SceneViewer>>>() }
	{
		//Initialize IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(window, installCallbacks);
		ImGui_ImplOpenGL3_Init(version.c_str());
	}

	Impl(Impl&& rhs) = delete;
	Impl& operator=(Impl&& rhs) = delete;

	Impl(const Impl& rhs) = delete;
	Impl& operator=(const Impl& rhs) = delete;

	void Update(const bool& isMouseHidden)
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

			auto isSelected = false;
			auto isHideMouse = false;

			{
				auto isHovered = false;
				auto isFocused = ImGui::IsWindowFocused();
				auto isDragging = ImGui::IsMouseDragging(ImGuiMouseButton_Left);

				if (isFocused)
				{
					++isFocusedCnt;

					if (!isNoMouse)
					{
						ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NoMouseCursorChange;
					}
				}

				if (m_sceneList->at(i)->GetViewerType() == InGame)
				{
					isSelected = isFocused && !isDragging;
					isHideMouse = isSelected;;
				}
				else if (m_sceneList->at(i)->GetViewerType() == Editor)
				{
					isHideMouse = isFocused && ImGui::IsMouseDown(ImGuiMouseButton_Middle);

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

					isSelected = isFocused && isHovered && !isDragging;
				}
			}

			m_sceneList->at(i)->InvokeSelectCallback(isSelected, isHideMouse);

			ImGui::End();
		}

		if (isFocusedCnt == 0 && isNoMouse)
		{
			ImGui::GetIO().ConfigFlags &= ~(ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NoMouseCursorChange);
		}

		if (isHoveredCnt == 0 && !isMouseHidden)
		{
			ImGui::SetWindowFocus(nullptr);
		}

		ImGui::Begin("Hierarchy");
		ImGui::Text("Put Scene Hierarchy Here!!!!");
		ImGui::SetWindowSize(ImVec2(ScreenWidth / 2, ScreenHeight / 2));
		ImGui::End();
	}

	void EndUpdate()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void AddSceneViewers(GLuint sceneTex, std::string sceneName, SceneViewerType viewerType, std::function<void(bool, bool)> selectCallback)
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

EngineUIMain::EngineUIMain(GLFWwindow* window, const bool installCallbacks, const std::string version)
	: m_pImpl{ std::make_unique<Impl>(window , installCallbacks, version )}
{
}

void EngineUIMain::Update(const bool& isMouseHidden)
{
	Pimpl()->Update(isMouseHidden);
}

void EngineUIMain::EndUpdate()
{
	Pimpl()->EndUpdate();
}

void EngineUIMain::AddSceneViewers(GLuint sceneTex, std::string sceneName, SceneViewerType viewerType, std::function<void(bool, bool)> selectCallback)
{
	Pimpl()->AddSceneViewers(sceneTex, sceneName, viewerType, selectCallback);
}

EngineUIMain::~EngineUIMain() = default;
