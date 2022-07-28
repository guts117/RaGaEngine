#ifndef RENDER_ENGINE_MAIN
#define RENDER_ENGINE_MAIN

#include "pch.h"
//#include "spimpl.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class RenderEngineMain
		{
		public:
			explicit RenderEngineMain();

			GLFWwindow* Init();

			void Update();
			void EndUpdate();

			bool IsEnd();

			~RenderEngineMain();

			RenderEngineMain(RenderEngineMain&& rhs) noexcept = delete;
			RenderEngineMain& operator=(RenderEngineMain&& rhs) noexcept = delete;

			RenderEngineMain(const RenderEngineMain& rhs) noexcept = delete;
			RenderEngineMain& operator=(const RenderEngineMain& rhs) noexcept = delete;
		private:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl = std::make_unique<Impl>();
		};
	}
}
#endif