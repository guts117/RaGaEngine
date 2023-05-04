#ifndef SCENE_FBO_HANDLER_MANAGER
#define SCENE_FBO_HANDLER_MANAGER

#include "render_pch.h"

//Loads and manages all the framebuffers 

namespace NarakaRenderEngine
{
	namespace RenderEngine 
	{
		class Fbo_Handler;

		class Scene_Fbo_Handler_Manager
		{
		public:
			explicit Scene_Fbo_Handler_Manager() = delete;
			explicit Scene_Fbo_Handler_Manager(const std::string& sceneName, const glm::ivec2& screenDims);

			Scene_Fbo_Handler_Manager(Scene_Fbo_Handler_Manager&& rhs) noexcept;
			Scene_Fbo_Handler_Manager& operator=(Scene_Fbo_Handler_Manager&& rhs) noexcept;

			Scene_Fbo_Handler_Manager(const Scene_Fbo_Handler_Manager& rhs) noexcept = delete;
			Scene_Fbo_Handler_Manager& operator=(const Scene_Fbo_Handler_Manager& rhs) noexcept = delete;

			Fbo_Handler* FindFboHandler(const std::string& handlerName);
			void ResizeScreenFboHandlers(const GLuint& width, const GLuint& height);
			Fbo_Handler* AddGameCameraFboHandlers(const int& cameraId, const glm::ivec2& screenDims);
			~Scene_Fbo_Handler_Manager() noexcept;

		private:
			struct Impl;

			const Impl& Pimpl() const { return m_pImpl.Get(); }
			Impl& Pimpl() { return m_pImpl.Get(); }

#ifdef NDEBUG //size of string debug(40), release(32) and size of vector debug(32), release(24)
			ForwardDeclaredPimpl<Impl, alignof(std::string) * 7, alignof(std::string)> m_pImpl;
#else
			ForwardDeclaredPimpl<Impl, alignof(std::string) * 9, alignof(std::string)> m_pImpl;
#endif;
		};

	}
}
#endif