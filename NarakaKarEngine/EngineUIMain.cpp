#include "pch.h"
#include "EngineUIMain.h"

using namespace NarakaKarEngine;
using namespace EngineUI;

extern int ScreenWidth;
extern int ScreenHeight;

EngineUIMain::EngineUIMain() = default;

EngineUIMain::EngineUIMain(GLFWwindow* window, const bool installCallbacks, const std::string version)
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
}

void EngineUIMain::EndUpdate()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

EngineUIMain::~EngineUIMain() 
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
};
