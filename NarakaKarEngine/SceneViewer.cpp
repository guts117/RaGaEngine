#include "pch.h"
#include "SceneViewer.h"
#include "Texture.h"

NarakaKarEngine::EngineUI::SceneViewer::SceneViewer(GLuint sceneTex, std::string sceneName, SceneViewerType viewerType, std::function<void(bool, bool)> selectCallback)
	: m_textureID{sceneTex}
	, m_viewerName{sceneName}
	, m_viewerType{viewerType}
	, m_SelectSceneCallback {selectCallback}
{
}

