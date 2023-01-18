#include "creator_pch.h"
#include "EngineUIMain.h"
#include "SceneViewer.h"


using namespace NarakaCreator;
using namespace EngineUI;

struct SceneViewer::Impl
{
	GLuint m_textureID;
	std::string m_viewerName;
	SceneViewerType m_viewerType = SceneViewerType::Empty;
	std::function<void(bool)> m_SelectSceneCallback;

	Impl() = delete;

	Impl(GLuint sceneTex, std::string sceneName, SceneViewerType viewerType, std::function<void(bool)> selectCallback)
		: m_textureID{ sceneTex }
		, m_viewerName{ sceneName }
		, m_viewerType{ viewerType }
		, m_SelectSceneCallback{ selectCallback }
	{
	}

	Impl(Impl&& rhs) = delete;
	Impl& operator=(Impl&& rhs) = delete;

	Impl(const Impl& rhs) = delete;
	Impl& operator=(const Impl& rhs) = delete;

	GLuint GetTextureId() { return m_textureID; }
	std::string GetViewerName() { return m_viewerName; }
	SceneViewerType GetViewerType() { return m_viewerType; }
	void InvokeSelectCallback(bool isSelected) { m_SelectSceneCallback(isSelected); }

	~Impl() = default;
};



SceneViewer::SceneViewer() = default;

SceneViewer::SceneViewer(GLuint sceneTex, std::string sceneName, SceneViewerType viewerType, std::function<void(bool)> selectCallback)
	: m_pImpl{ std::make_unique<Impl>(sceneTex, sceneName, viewerType, selectCallback) }
{
}

GLuint SceneViewer::GetTextureId() { return Pimpl()->GetTextureId(); }

std::string SceneViewer::GetViewerName() { return Pimpl()->GetViewerName(); }

SceneViewerType SceneViewer::GetViewerType() { return Pimpl()->GetViewerType(); }

void SceneViewer::InvokeSelectCallback(bool isSelected) { Pimpl()->InvokeSelectCallback(isSelected); }

SceneViewer::~SceneViewer() = default;