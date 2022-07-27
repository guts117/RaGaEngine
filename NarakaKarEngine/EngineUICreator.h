#ifndef ENGINE_UI_CREATOR
#define ENGINE_UI_CREATOR

#include "pch.h"

namespace NarakaKarEngine 
{
	namespace EngineUI
	{
		class EngineUICreator
		{
		public:
			explicit EngineUICreator();
			explicit EngineUICreator(GLFWwindow* window, const bool installCallbacks, const std::string version);

			EngineUICreator(EngineUICreator&& rhs) noexcept = default;
			EngineUICreator& operator=(EngineUICreator&& rhs) noexcept = default;

			EngineUICreator(const EngineUICreator& rhs) noexcept = delete;
			EngineUICreator& operator=(const EngineUICreator& rhs) noexcept = delete;

			void Update();

			void Render();

			~EngineUICreator();
		};
	}
}

#endif
