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
	auto io = ImGui::GetIO();
	(void)io;
	ImGui_ImplGlfw_InitForOpenGL(window, installCallbacks);
	ImGui_ImplOpenGL3_Init(version.c_str());
}

void EngineUIMain::Update()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Welcome to NarakaKarEngine");
	ImGui::Text("Hello Game World!!!!");
	ImGui::SetWindowSize(ImVec2(ScreenWidth / 2, ScreenHeight / 2));
	ImGui::End();

	for (int i = 0; i < m_sceneList->size(); ++i)
	{
		ImGui::Begin(m_sceneList->at(i)->GetViewerName().c_str());
		ImGui::SetWindowSize(ImVec2(ScreenWidth / 2, ScreenHeight / 2));
		ImGui::Image((ImTextureID)m_sceneList->at(i)->GetTextureId(), ImGui::GetWindowSize(), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();
	}
}

void EngineUIMain::EndUpdate()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EngineUIMain::AddSceneViewers(GLuint sceneTex, std::string sceneName, SceneViewerType viewerType)
{
	auto scene = std::make_shared<SceneViewer>(sceneTex, sceneName, viewerType);
	m_sceneList->push_back(scene);
}

EngineUIMain::~EngineUIMain() 
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
};
