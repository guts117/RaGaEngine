#ifndef ENGINE_UI_MAIN
#define ENGINE_UI_MAIN

#include "pch.h"
#include "RenderingCommonValues.h"

namespace NarakaKarEngine 
{
	namespace EngineUI
	{
		class SceneViewer;

		class EngineUIMain
		{
		public:
			explicit EngineUIMain();
			explicit EngineUIMain(GLFWwindow* window, const bool installCallbacks, const std::string version);

			EngineUIMain(EngineUIMain&& rhs) noexcept = default;
			EngineUIMain& operator=(EngineUIMain&& rhs) noexcept = default;

			EngineUIMain(const EngineUIMain& rhs) noexcept = delete;
			EngineUIMain& operator=(const EngineUIMain& rhs) noexcept = delete;

			void Update();
			void EndUpdate();

			void AddSceneViewers(GLuint sceneTex, std::string sceneName, SceneViewerType viewerType);

			~EngineUIMain();
		private: 
			std::unique_ptr<std::vector<std::shared_ptr<SceneViewer>>> m_sceneList;
		};
	}
}

#endif
