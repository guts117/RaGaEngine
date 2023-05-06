#ifndef SCENE_VIEWER
#define SCENE_VIEWER

#include "creator_pch.h"
#include <ForwardDeclaredPimpl.h>

namespace NarakaCreator
{
	namespace EngineUI
	{
		enum SceneViewerType;

		class SceneViewer
		{
		public:
			explicit SceneViewer() = delete;
			explicit SceneViewer(GLuint sceneTex, std::string sceneName, SceneViewerType viewerType, std::function<void(bool)> selectCallback);

			SceneViewer(SceneViewer&& rhs) noexcept;
			SceneViewer& operator=(SceneViewer&& rhs) noexcept;

			SceneViewer(const SceneViewer& rhs) noexcept = delete;
			SceneViewer& operator=(const SceneViewer& rhs) noexcept = delete;

			GLuint GetTextureId();
			std::string GetViewerName();
			SceneViewerType GetViewerType();
			void InvokeSelectCallback(bool isSelected);

			~SceneViewer() noexcept;
		private:
			struct Impl;

			const Impl& Pimpl() const { return m_pImpl.Get(); }
			Impl& Pimpl() { return m_pImpl.Get(); }

#ifdef NDEBUG //size of string is debug(40) and release(32), function<> is debug/release(64)
			ForwardDeclaredPimpl<Impl, alignof(void*) * 13, alignof(void*)> m_pImpl;
#else
			ForwardDeclaredPimpl<Impl, alignof(void*) * 14, alignof(void*)> m_pImpl;
#endif;
		};
	}
}

#endif

