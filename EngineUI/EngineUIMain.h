#ifndef ENGINE_UI_MAIN
#define ENGINE_UI_MAIN

#ifdef ENGINE_UI_EXPORTS
#define ENGINE_UI_API __declspec(dllexport)
#else
#define ENGINE_UI_API __declspec(dllimport)
#endif

#include "engine_ui_pch.h"

namespace NarakaKarEngine
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

		class ENGINE_UI_API EngineUIMain
		{
		public:
			explicit EngineUIMain() = delete;
			explicit EngineUIMain(const bool installCallbacks, const std::string version);

			EngineUIMain(EngineUIMain&& rhs) noexcept = delete;
			EngineUIMain& operator=(EngineUIMain&& rhs) noexcept = delete;

			EngineUIMain(const EngineUIMain& rhs) noexcept = delete;
			EngineUIMain& operator=(const EngineUIMain& rhs) noexcept = delete;

			void CreateInputCallbacks(const GLFWkeyfun& keyCallback, const GLFWcursorposfun& cursorPosCallback, const GLFWmousebuttonfun& mouseBtnCallback, const GLFWscrollfun& mouseScrollCallback);
			void Update(const glm::ivec2& screenDims);
			void EndUpdate();

			void AddSceneViewers(GLuint sceneTex, std::string sceneName, SceneViewerType viewerType, std::function<void(bool)> selectCallback);
			GLFWwindow* GetMainWindow();
			glm::ivec2 GetScreenDimensions();
			bool IsUpdateBufferSize();

			bool IsEnd();

			~EngineUIMain();
		private:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}

#endif


