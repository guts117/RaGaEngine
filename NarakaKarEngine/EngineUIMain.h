#ifndef ENGINE_UI_MAIN
#define ENGINE_UI_MAIN

#include "pch.h"

namespace NarakaKarEngine 
{
	namespace EngineUI
	{
		class EngineUIMain
		{
		public:
			explicit EngineUIMain();
			explicit EngineUIMain(GLFWwindow* window, const bool installCallbacks, const std::string version);

			EngineUIMain(EngineUIMain&& rhs) noexcept = default;
			EngineUIMain& operator=(EngineUIMain&& rhs) noexcept = default;

			EngineUIMain(const EngineUIMain& rhs) noexcept = delete;
			EngineUIMain& operator=(const EngineUIMain& rhs) noexcept = delete;

			void Update();

			void EndUpdate();

			~EngineUIMain();
		};
	}
}

#endif
