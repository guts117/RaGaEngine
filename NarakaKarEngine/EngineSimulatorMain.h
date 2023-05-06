#ifndef ENGINE_SIMULATOR
#define ENGINE_SIMULATOR

#include "pch.h"

namespace NarakaKarEngine
{
	class EngineSimulatorMainMain
	{
	public:
		explicit EngineSimulatorMainMain() noexcept = delete;
		//explicit EngineSimulatorMainMain(const glm::ivec2& screenDims);

		void Update(const glm::ivec2& screenDims);
		void EndUpdate();

		EngineSimulatorMainMain(EngineSimulatorMainMain&& rhs) noexcept = delete;
		EngineSimulatorMainMain& operator=(EngineSimulatorMainMain&& rhs) noexcept = delete;

		EngineSimulatorMainMain(const EngineSimulatorMainMain& rhs) noexcept = delete;
		EngineSimulatorMainMain& operator=(const EngineSimulatorMainMain& rhs) noexcept = delete;

		void AddToUpdateStack(std::function<void(glm::ivec2)> updateFunc);

		~EngineSimulatorMainMain() noexcept;
	private:
		struct Impl;

		const Impl* Pimpl() const { return m_pImpl.get(); }
		Impl* Pimpl() { return m_pImpl.get(); }

		std::unique_ptr<Impl> m_pImpl;
	};
}

#endif