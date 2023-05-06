#include "creator_pch.h"
#include "EngineUIMain.h"
#include "SceneViewer.h"
#include "Window.h"
#include "EngineInputManager.h"
#include <imgui_node_editor.h>

namespace ed = ax::NodeEditor;

using namespace NarakaCreator;
using namespace EngineUI;
using namespace InputManager;

extern int ScreenWidth;
extern int ScreenHeight;
extern bool IsUpdateFrameBuffersSize;

//ToDo: This is copy pasted from Example basic-interaction-example.cpp
struct EngineUIMain::SimpleNodeEditorExample
{
    void Init()
    {
		ed::Config config;
		config.SettingsFile = "Simple.json";
		m_Context = ed::CreateEditor(&config);
    }

    void End()
    {
        ed::DestroyEditor(m_Context);
    }

    void ImGuiEx_BeginColumn()
    {
        ImGui::BeginGroup();
    }

    void ImGuiEx_NextColumn()
    {
        ImGui::EndGroup();
        ImGui::SameLine();
        ImGui::BeginGroup();
    }

    void ImGuiEx_EndColumn()
    {
        ImGui::EndGroup();
    }

    void Update()
    {
		auto& io = ImGui::GetIO();

		ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);

		ImGui::Separator();

		ed::SetCurrentEditor(m_Context);
		ed::Begin("SimpleNodeEditorExample", ImVec2(0.0, 0.0f));
		int uniqueId = 1;
		// Start drawing nodes.
		ed::BeginNode(uniqueId++);
		ImGui::Text("Node A");
		ed::BeginPin(uniqueId++, ed::PinKind::Input);
		ImGui::Text("-> In");
		ed::EndPin();
		ImGui::SameLine();
		ed::BeginPin(uniqueId++, ed::PinKind::Output);
		ImGui::Text("Out ->");
		ed::EndPin();
		ed::EndNode();
		ed::End();
		ed::SetCurrentEditor(nullptr);

		//ImGui::ShowMetricsWindow();
    }

    ed::EditorContext* m_Context = nullptr;    // Editor context, required to trace a editor state.
};

struct EngineUIMain::BasicInteractionNodeEditorExample
{
    // Struct to hold basic information about connection between
    // pins. Note that connection (aka. link) has its own ID.
    // This is useful later with dealing with selections, deletion
    // or other operations.
    struct LinkInfo
    {
        ed::LinkId Id;
        ed::PinId  InputId;
        ed::PinId  OutputId;
    };

    void Init()
    {
        ed::Config config;
        config.SettingsFile = "BasicInteraction.json";
        m_Context = ed::CreateEditor(&config);
    }

    void End()
    {
        ed::DestroyEditor(m_Context);
    }

    void ImGuiEx_BeginColumn()
    {
        ImGui::BeginGroup();
    }

    void ImGuiEx_NextColumn()
    {
        ImGui::EndGroup();
        ImGui::SameLine();
        ImGui::BeginGroup();
    }

    void ImGuiEx_EndColumn()
    {
        ImGui::EndGroup();
    }

    void Update()
    {
        auto& io = ImGui::GetIO();

        ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);

        ImGui::Separator();

        ed::SetCurrentEditor(m_Context);

        // Start interaction with editor.
        ed::Begin("My Editor", ImVec2(0.0, 0.0f));

        int uniqueId = 1;

        //
        // 1) Commit known data to editor
        //

        // Submit Node A
        ed::NodeId nodeA_Id = uniqueId++;
        ed::PinId  nodeA_InputPinId = uniqueId++;
        ed::PinId  nodeA_OutputPinId = uniqueId++;

        if (m_FirstFrame)
            ed::SetNodePosition(nodeA_Id, ImVec2(10, 10));
        ed::BeginNode(nodeA_Id);
        ImGui::Text("Node A");
        ed::BeginPin(nodeA_InputPinId, ed::PinKind::Input);
        ImGui::Text("-> In");
        ed::EndPin();
        ImGui::SameLine();
        ed::BeginPin(nodeA_OutputPinId, ed::PinKind::Output);
        ImGui::Text("Out ->");
        ed::EndPin();
        ed::EndNode();

        // Submit Node B
        ed::NodeId nodeB_Id = uniqueId++;
        ed::PinId  nodeB_InputPinId1 = uniqueId++;
        ed::PinId  nodeB_InputPinId2 = uniqueId++;
        ed::PinId  nodeB_OutputPinId = uniqueId++;

        if (m_FirstFrame)
            ed::SetNodePosition(nodeB_Id, ImVec2(210, 60));
        ed::BeginNode(nodeB_Id);
        ImGui::Text("Node B");
        ImGuiEx_BeginColumn();
        ed::BeginPin(nodeB_InputPinId1, ed::PinKind::Input);
        ImGui::Text("-> In1");
        ed::EndPin();
        ed::BeginPin(nodeB_InputPinId2, ed::PinKind::Input);
        ImGui::Text("-> In2");
        ed::EndPin();
        ImGuiEx_NextColumn();
        ed::BeginPin(nodeB_OutputPinId, ed::PinKind::Output);
        ImGui::Text("Out ->");
        ed::EndPin();
        ImGuiEx_EndColumn();
        ed::EndNode();

        // Submit Links
        for (auto& linkInfo : m_Links)
            ed::Link(linkInfo.Id, linkInfo.InputId, linkInfo.OutputId);

        //
        // 2) Handle interactions
        //

        // Handle creation action, returns true if editor want to create new object (node or link)
        if (ed::BeginCreate())
        {
            ed::PinId inputPinId, outputPinId;
            if (ed::QueryNewLink(&inputPinId, &outputPinId))
            {
                // QueryNewLink returns true if editor want to create new link between pins.
                //
                // Link can be created only for two valid pins, it is up to you to
                // validate if connection make sense. Editor is happy to make any.
                //
                // Link always goes from input to output. User may choose to drag
                // link from output pin or input pin. This determine which pin ids
                // are valid and which are not:
                //   * input valid, output invalid - user started to drag new ling from input pin
                //   * input invalid, output valid - user started to drag new ling from output pin
                //   * input valid, output valid   - user dragged link over other pin, can be validated

                if (inputPinId && outputPinId) // both are valid, let's accept link
                {
                    // ed::AcceptNewItem() return true when user release mouse button.
                    if (ed::AcceptNewItem())
                    {
                        // Since we accepted new link, lets add one to our list of links.
                        m_Links.push_back({ ed::LinkId(m_NextLinkId++), inputPinId, outputPinId });

                        // Draw new link.
                        ed::Link(m_Links.back().Id, m_Links.back().InputId, m_Links.back().OutputId);
                    }

                    // You may choose to reject connection between these nodes
                    // by calling ed::RejectNewItem(). This will allow editor to give
                    // visual feedback by changing link thickness and color.
                }
            }
        }
        ed::EndCreate(); // Wraps up object creation action handling.


        // Handle deletion action
        if (ed::BeginDelete())
        {
            // There may be many links marked for deletion, let's loop over them.
            ed::LinkId deletedLinkId;
            while (ed::QueryDeletedLink(&deletedLinkId))
            {
                // If you agree that link can be deleted, accept deletion.
                if (ed::AcceptDeletedItem())
                {
                    // Then remove link from your data.
                    for (auto& link : m_Links)
                    {
                        if (link.Id == deletedLinkId)
                        {
                            m_Links.erase(&link);
                            break;
                        }
                    }
                }

                // You may reject link deletion by calling:
                // ed::RejectDeletedItem();
            }
        }
        ed::EndDelete(); // Wrap up deletion action



        // End of interaction with editor.
        ed::End();

        if (m_FirstFrame)
            ed::NavigateToContent(0.0f);

        ed::SetCurrentEditor(nullptr);

        m_FirstFrame = false;

        // ImGui::ShowMetricsWindow();
    }

    ed::EditorContext* m_Context = nullptr;    // Editor context, required to trace a editor state.
    bool                 m_FirstFrame = true;    // Flag set for first frame only, some action need to be executed once.
    ImVector<LinkInfo>   m_Links;                // List of live links. It is dynamic unless you want to create read-only view over nodes.
    int                  m_NextLinkId = 100;     // Counter to help generate link ids. In real application this will probably based on pointer to user data structure.
};

struct EngineUIMain::Impl
{
	int hideWindowIndex;
	std::unique_ptr<Window> mainWindow;
	std::vector<SceneViewer> m_sceneList;

    //ToDo: Start doing Node/Graph Editor Stuff
    SimpleNodeEditorExample nodeEditorEg;
    BasicInteractionNodeEditorExample interactionNodeEditorEg;
    
    

	Impl() = delete;

	Impl(const bool installCallbacks, const std::string version)
		: m_sceneList{ std::vector<SceneViewer>() }
		, mainWindow{ std::make_unique<Window>() }
	{		
		mainWindow->Initialise();
		mainWindow->CreateInputCallbacks(EngineInputManager::HandleKeysPresses, EngineInputManager::HandleCursorPosition, EngineInputManager::HandleMousePresses, EngineInputManager::HandleMouseScrolls);

		//Initialize IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(mainWindow->GetWindow(), installCallbacks);
		ImGui_ImplOpenGL3_Init(version.c_str());
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

        //ToDo:
        nodeEditorEg = SimpleNodeEditorExample();
        nodeEditorEg.Init();

        interactionNodeEditorEg = BasicInteractionNodeEditorExample();
        interactionNodeEditorEg.Init();
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

		for (int i = 0; i < m_sceneList.size(); ++i)
		{
			ImGui::Begin(m_sceneList[i].GetViewerName().c_str(), nullptr, ImGuiWindowFlags_NoFocusOnAppearing);
			ImGui::SetWindowSize(ImVec2(screenDims.x / 2, screenDims.y / 2));
			ImGui::Image((ImTextureID)m_sceneList[i].GetTextureId(), ImGui::GetWindowSize(), ImVec2(0, 1), ImVec2(1, 0));

			auto isSelected = false;
			auto isDragging = ImGui::IsMouseDragging(ImGuiMouseButton_Left);

			if (!isDragging)
			{
				auto pos = ImGui::GetWindowPos();
				auto maxx = pos.x + ImGui::GetWindowWidth();
				auto maxy = pos.y + ImGui::GetWindowHeight();
				auto isHovered = ImGui::IsMouseHoveringRect(pos, ImVec2(maxx, maxy));
				auto isFocused = ImGui::IsWindowDocked() ? ImGui::IsWindowFocused() : true;

				if (m_sceneList[i].GetViewerType() == Editor && hideWindowIndex < 0)
				{
					isSelected = isHovered && isFocused;
					if (isSelected)
					{
						ImGui::SetWindowFocus(m_sceneList[i].GetViewerName().c_str());
					}
				}
				else if(m_sceneList[i].GetViewerType() == InGame && hideWindowIndex == i || hideWindowIndex < 0)
				{
					isSelected = isHovered && isFocused;
					if (isSelected) 
					{
						++isHoveredCnt;
						hideWindowIndex = i;
						ImGui::SetWindowFocus(m_sceneList[i].GetViewerName().c_str());
						ImGui::GetIO().WantCaptureMouse = false;
					}
				}
			}

			m_sceneList[i].InvokeSelectCallback(isSelected);

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

		ImGui::Begin("Hierarchy");
		ImGui::Text("ToDo: Show World Hierarchy Here!!!!");
		ImGui::SetWindowSize(ImVec2(screenDims.x / 2, screenDims.y / 2));
		ImGui::End();

        //ToDo: Note Seems like you can't open two node editor window at the same time.
        //nodeEditorEg.Update();
        interactionNodeEditorEg.Update();
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
		auto scene = SceneViewer(sceneTex, sceneName, viewerType, selectCallback);
		m_sceneList.emplace_back(std::move(scene));
	}

	~Impl()
	{
        nodeEditorEg.End();
        interactionNodeEditorEg.End();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
};

EngineUIMain::EngineUIMain(const bool installCallbacks, const std::string version)
	: m_pImpl{ new Impl(installCallbacks, version) }
{
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