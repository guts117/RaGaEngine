#ifndef ENGINE_UI_MAIN
#define ENGINE_UI_MAIN

#ifdef NARAKA_CREATOR_EXPORTS
#define NARAKA_CREATOR_API __declspec(dllexport)
#else
#define NARAKA_CREATOR_API __declspec(dllimport)
#endif

#include "creator_pch.h"

namespace NarakaCreator
{
	namespace EngineUI
	{
		enum SceneViewerType
		{
			Empty,
			Editor,
			InGame,
			//Add other sceneviewers for debug purposes too 
		};

		class NARAKA_CREATOR_API EngineUIMain
		{
		public:
			explicit EngineUIMain() noexcept = delete;
			explicit EngineUIMain(const bool installCallbacks, const std::string version) noexcept;

			EngineUIMain(EngineUIMain&& rhs) noexcept = delete;
			EngineUIMain& operator=(EngineUIMain&& rhs) noexcept = delete;

			EngineUIMain(const EngineUIMain& rhs) noexcept = delete;
			EngineUIMain& operator=(const EngineUIMain& rhs) noexcept = delete;

			void Update(const glm::ivec2& screenDims);
			void EndUpdate();

			void AddSceneViewers(GLuint sceneTex, std::string sceneName, SceneViewerType viewerType, std::function<void(bool)> selectCallback);
			GLFWwindow* GetMainWindow();
			glm::ivec2 GetScreenDimensions();
			bool IsUpdateBufferSize();
			static bool AddKeyBoardButtonEvent(int key, bool down);
			static bool AddCursorPosEvent(float x, float y);
			static bool AddMouseButtonEvent(int mouse_button, bool down);
			static bool AddMouseScrollEvent(float wheel_x, float wheel_y);

			bool IsEnd();

			~EngineUIMain() noexcept;
		private:
			struct Impl;
			struct SimpleNodeEditorExample;
			struct BasicInteractionNodeEditorExample;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}

#endif


