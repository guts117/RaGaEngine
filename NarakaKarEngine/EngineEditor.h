#ifndef ENGINE_EDITOR
#define ENGINE_EDITOR

#include "pch.h"

namespace NarakaKarEngine
{
	namespace EngineUI
	{
		class EngineUIMain;
	}

	namespace NarakaEditor
	{
		class EngineEditor
		{
		public:
			explicit EngineEditor() = delete;
			explicit EngineEditor(const bool installCallbacks, const std::string version);

			EngineEditor(EngineEditor&& rhs) noexcept = delete;
			EngineEditor& operator=(EngineEditor&& rhs) noexcept = delete;

			EngineEditor(const EngineEditor& rhs) noexcept = delete;
			EngineEditor& operator=(const EngineEditor& rhs) noexcept = delete;

			void Update(const glm::ivec2& screenDims);
			void EndUpdate();

			void AddViewers(EngineUI::EngineUIMain* engineUI);

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
