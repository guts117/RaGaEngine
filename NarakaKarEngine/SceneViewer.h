#ifndef SCENE_VIEWER
#define SCENE_VIEWER

#include "pch.h"
#include "RenderingCommonValues.h"

namespace NarakaKarEngine
{
	namespace EngineUI
	{
		class SceneViewer
		{
		public:
			explicit SceneViewer() = default;
			explicit SceneViewer(GLuint sceneTex, std::string sceneName, SceneViewerType viewerType);

			GLuint GetTextureId()						{ return m_textureID; }
			std::string GetViewerName()				{ return m_viewerName; }
			SceneViewerType GetViewerType()			{ return m_viewerType; }

		private:
			GLuint m_textureID;
			std::string m_viewerName;
			SceneViewerType m_viewerType = SceneViewerType::Empty;
		};
	}
}

#endif

