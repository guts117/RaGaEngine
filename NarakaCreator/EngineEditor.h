#ifndef ENGINE_EDITOR
#define ENGINE_EDITOR

#ifdef NARAKA_CREATOR_EXPORTS
#define NARAKA_CREATOR_API __declspec(dllexport)
#else
#define NARAKA_CREATOR_API __declspec(dllimport)
#endif

#include "creator_pch.h"

namespace NarakaRenderEngine::RenderEngine
{
	class RenderEngineMain;
}

namespace NarakaCreator
{
	namespace EngineUI
	{
		class SceneViewer;
		class EngineUIMain;
	}

	struct RendererToViewer
	{
		std::string sceneFboName;
		std::string viewerName;
		int viewerType;				//SceneViewerType
		int fboIndex;
		int bufferIndex;
	};

	class NARAKA_CREATOR_API EngineEditor
	{
	public:
		explicit EngineEditor();

		EngineEditor(EngineEditor&& rhs) noexcept = delete;
		EngineEditor& operator=(EngineEditor&& rhs) noexcept = delete;

		EngineEditor(const EngineEditor& rhs) noexcept = delete;
		EngineEditor& operator=(const EngineEditor& rhs) noexcept = delete;
		
		bool IsUpdateBufferSize();
		glm::ivec2 GetScreenDimensions();
		bool IsEnd();

		void Update(const glm::ivec2& screenDims);
		void EndUpdate();

		void AddSceneViewers(const NarakaRenderEngine::RenderEngine::RenderEngineMain* renderEngineMain);

		~EngineEditor();
	private:
		struct Impl;

		const Impl* Pimpl() const { return m_pImpl.get(); }
		Impl* Pimpl() { return m_pImpl.get(); }

		std::unique_ptr<Impl> m_pImpl;
	};
}

#endif
