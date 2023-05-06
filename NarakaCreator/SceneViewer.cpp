#include "creator_pch.h"
#include "EngineUIMain.h"
#include "SceneViewer.h"


using namespace NarakaCreator;
using namespace EngineUI;

struct alignas(alignof(void*)) SceneViewer::Impl
{
	std::function<void(bool)> m_SelectSceneCallback;
	std::string m_viewerName;
	GLuint m_textureID;
	SceneViewerType m_viewerType = SceneViewerType::Empty;

	Impl() = delete;

	Impl(GLuint sceneTex, std::string sceneName, SceneViewerType viewerType, std::function<void(bool)> selectCallback)
		: m_SelectSceneCallback{ selectCallback }	
		, m_viewerName{ sceneName }
		, m_textureID{ sceneTex }
		, m_viewerType{ viewerType }
	{
	}

	Impl(Impl&& rhs) noexcept = default;
	Impl& operator=(Impl&& rhs) noexcept = default;

	Impl(const Impl& rhs) noexcept = delete;
	Impl& operator=(const Impl& rhs) noexcept = delete;

	GLuint GetTextureId() { return m_textureID; }
	std::string GetViewerName() { return m_viewerName; }
	SceneViewerType GetViewerType() { return m_viewerType; }
	void InvokeSelectCallback(bool isSelected) { m_SelectSceneCallback(isSelected); }

	~Impl() noexcept = default;
};

SceneViewer::SceneViewer(GLuint sceneTex, std::string sceneName, SceneViewerType viewerType, std::function<void(bool)> selectCallback)
	: m_pImpl{ Impl(sceneTex, sceneName, viewerType, selectCallback) }
{
}

SceneViewer::SceneViewer(SceneViewer&& rhs) noexcept = default;
SceneViewer& SceneViewer::operator=(SceneViewer&& rhs) noexcept = default;

GLuint SceneViewer::GetTextureId() { return Pimpl().GetTextureId(); }

std::string SceneViewer::GetViewerName() { return Pimpl().GetViewerName(); }

SceneViewerType SceneViewer::GetViewerType() { return Pimpl().GetViewerType(); }

void SceneViewer::InvokeSelectCallback(bool isSelected) { Pimpl().InvokeSelectCallback(isSelected); }

SceneViewer::~SceneViewer() noexcept = default;