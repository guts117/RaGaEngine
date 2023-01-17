#ifndef SCENE_VIEWER
#define SCENE_VIEWER

#include "engine_ui_pch.h"

namespace NarakaKarEngine
{
	namespace EngineUI
	{
		enum SceneViewerType;

		class SceneViewer
		{
		public:
			explicit SceneViewer();
			explicit SceneViewer(GLuint sceneTex, std::string sceneName, SceneViewerType viewerType, std::function<void(bool)> selectCallback);

			GLuint GetTextureId();
			std::string GetViewerName();
			SceneViewerType GetViewerType();
			void InvokeSelectCallback(bool isSelected);

			~SceneViewer();
		private:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}

#endif

