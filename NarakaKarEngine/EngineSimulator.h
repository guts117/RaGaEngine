#ifndef ENGINE_SIMULATOR
#define ENGINE_SIMULATOR

#include "pch.h"

namespace NarakaKarEngine
{
	class EngineSimulatorMain
	{
	public:
		explicit EngineSimulatorMain() = delete;
		//explicit EngineSimulatorMain(const glm::ivec2& screenDims);

		void Update(const glm::ivec2& screenDims);
		void EndUpdate();

		EngineSimulatorMain(EngineSimulatorMain&& rhs) noexcept = delete;
		EngineSimulatorMain& operator=(EngineSimulatorMain&& rhs) noexcept = delete;

		EngineSimulatorMain(const EngineSimulatorMain& rhs) noexcept = delete;
		EngineSimulatorMain& operator=(const EngineSimulatorMain& rhs) noexcept = delete;

		~EngineSimulatorMain();
	private:
		struct Impl;

		const Impl* Pimpl() const { return m_pImpl.get(); }
		Impl* Pimpl() { return m_pImpl.get(); }

		std::unique_ptr<Impl> m_pImpl;
	};
}

#endif