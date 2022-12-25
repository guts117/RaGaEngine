#ifndef RENDER_ENGINE_MAIN
#define RENDER_ENGINE_MAIN

#include "pch.h"
//#include "spimpl.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		struct Transform
		{
			glm::vec3 Position;
			glm::vec3 Rotation;
			glm::vec3 Scale;
		};

		class RenderEngineMain
		{
		public:
			explicit RenderEngineMain();

			void Update();
			void EndUpdate();

			GLFWwindow* GetMainWindow();
			void AddViewers();

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

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}
#endif