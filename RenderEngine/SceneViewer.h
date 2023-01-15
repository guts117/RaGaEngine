#ifndef SCENE_VIEWER
#define SCENE_VIEWER

#include "render_pch.h"
#include "RenderingCommonValues.h"

namespace NarakaKarEngine
{
	namespace EngineUI
	{
		class SceneViewer
		{
		public:
			explicit SceneViewer() = default;
			explicit SceneViewer(GLuint sceneTex, std::string sceneName, SceneViewerType viewerType, std::function<void(bool, bool)> selectCallback);

			GLuint GetTextureId()						{ return m_textureID; }
			std::string GetViewerName()					{ return m_viewerName; }
			SceneViewerType GetViewerType()				{ return m_viewerType; }
			void InvokeSelectCallback(bool isSelected, bool isHideCursor)	{ m_SelectSceneCallback(isSelected, isHideCursor); }

		private:
			GLuint m_textureID;
			std::string m_viewerName;
			SceneViewerType m_viewerType = SceneViewerType::Empty;
			std::function<void(bool, bool)> m_SelectSceneCallback;
		};
	}
}

#endif

