#ifndef ENGINE_UI_MAIN
#define ENGINE_UI_MAIN

#include "render_pch.h"
#include "RenderingCommonValues.h"

namespace NarakaKarEngine 
{
	namespace EngineUI
	{
		class SceneViewer;

		class EngineUIMain final
		{
		public:
			explicit EngineUIMain() = delete;
			explicit EngineUIMain(GLFWwindow* window, const bool installCallbacks, const std::string version);

			EngineUIMain(EngineUIMain&& rhs) noexcept = default;
			EngineUIMain& operator=(EngineUIMain&& rhs) noexcept = default;

			EngineUIMain(const EngineUIMain& rhs) noexcept = delete;
			EngineUIMain& operator=(const EngineUIMain& rhs) noexcept = delete;

			void Update(const bool& isMouseHidden);
			void EndUpdate();

			void AddSceneViewers(GLuint sceneTex, std::string sceneName, SceneViewerType viewerType, std::function<void(bool, bool)> selectCallback);

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
