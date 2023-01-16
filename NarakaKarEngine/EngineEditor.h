#ifndef ENGINE_EDITOR
#define ENGINE_EDITOR

#include "pch.h"

namespace NarakaKarEngine
{
	namespace EngineUI
	{
		class SceneViewer;
		class EngineUIMain;
	}

	namespace RenderEngine
	{
		class RenderEngineMain;
	}

	struct RendererToViewer
	{
		std::string sceneFboName;
		std::string viewerName;
		int viewerType;				//SceneViewerType
		int fboIndex;
		int bufferIndex;
	};

	namespace NarakaEditor
	{
		class EngineEditor
		{
		public:
			explicit EngineEditor() = delete;
			explicit EngineEditor(std::vector<RendererToViewer>&& render2Views);

			EngineEditor(EngineEditor&& rhs) noexcept = delete;
			EngineEditor& operator=(EngineEditor&& rhs) noexcept = delete;

			EngineEditor(const EngineEditor& rhs) noexcept = delete;
			EngineEditor& operator=(const EngineEditor& rhs) noexcept = delete;

			void Update();
			void EndUpdate();

			void AddSceneViewers(EngineUI::EngineUIMain* engineUI, const RenderEngine::RenderEngineMain* renderEngineMain);

			~EngineEditor();
		private:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}

#endif
