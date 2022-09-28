#include "pch.h"
#include "SceneViewer.h"
#include "Texture.h"

NarakaKarEngine::EngineUI::SceneViewer::SceneViewer(GLuint sceneTex, std::string sceneName, SceneViewerType viewerType)
	: m_textureID{sceneTex}
	, m_viewerName{sceneName}
	, m_viewerType{viewerType}
{
}

