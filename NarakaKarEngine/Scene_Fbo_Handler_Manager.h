#ifndef SCENE_FBO_HANDLER_MANAGER
#define SCENE_FBO_HANDLER_MANAGER

#include "pch.h"

namespace NarakaKarEngine
{
	namespace RenderEngine 
	{
		class Fbo_Handler;

		class Scene_Fbo_Handler_Manager
		{
		public:
			Scene_Fbo_Handler_Manager() = delete;
			explicit Scene_Fbo_Handler_Manager(const std::string& sceneName);

			Scene_Fbo_Handler_Manager(Scene_Fbo_Handler_Manager&& rhs) noexcept = default;
			Scene_Fbo_Handler_Manager& operator=(Scene_Fbo_Handler_Manager&& rhs) noexcept = default;

			Scene_Fbo_Handler_Manager(const Scene_Fbo_Handler_Manager& rhs) noexcept = delete;
			Scene_Fbo_Handler_Manager& operator=(const Scene_Fbo_Handler_Manager& rhs) noexcept = delete;

			std::shared_ptr<Fbo_Handler> FindFboHandler(const std::string& handlerName) const;
			void ResizeScreenFboHandlers(const GLuint& width, const GLuint& height);
			~Scene_Fbo_Handler_Manager();

		private:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};

	}
}
#endif