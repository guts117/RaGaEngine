#ifndef DEPTH_PASS_FBO_HANDLER
#define DEPTH_PASS_FBO_HANDLER

#include "pch.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Depth_Pass_Fbo_Handler
		{
		public:
			Depth_Pass_Fbo_Handler() = delete;
			explicit Depth_Pass_Fbo_Handler(const GLuint& width, const GLuint& height);

			Depth_Pass_Fbo_Handler(Depth_Pass_Fbo_Handler&& rhs) noexcept = default;
			Depth_Pass_Fbo_Handler& operator=(Depth_Pass_Fbo_Handler&& rhs) noexcept = default;

			Depth_Pass_Fbo_Handler(const Depth_Pass_Fbo_Handler& rhs) noexcept = delete;
			Depth_Pass_Fbo_Handler& operator=(const Depth_Pass_Fbo_Handler& rhs) noexcept = delete;

			void WriteToFBO() const;
			void AttachFBOToTextureUnit(const GLenum& textureUnit) const;
			void ResizeFBO(const GLuint& width, const GLuint& height);

			const GLuint& GetFBOWidth() const;
			const GLuint& GetFBOHeight() const;

			~Depth_Pass_Fbo_Handler();

		private:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}

#endif