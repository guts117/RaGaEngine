#ifndef MOTION_BLUR_PASS_FBO_HANDLER
#define MOTION_BLUR_PASS_FBO_HANDLER

#include "pch.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Motion_Blur_Pass_Fbo_Handler
		{
		public:
			Motion_Blur_Pass_Fbo_Handler() = delete;
			explicit Motion_Blur_Pass_Fbo_Handler(const GLuint& width, const GLuint& height);

			Motion_Blur_Pass_Fbo_Handler(Motion_Blur_Pass_Fbo_Handler&& rhs) noexcept = default;
			Motion_Blur_Pass_Fbo_Handler& operator=(Motion_Blur_Pass_Fbo_Handler&& rhs) noexcept = default;

			Motion_Blur_Pass_Fbo_Handler(const Motion_Blur_Pass_Fbo_Handler& rhs) noexcept = delete;
			Motion_Blur_Pass_Fbo_Handler& operator=(const Motion_Blur_Pass_Fbo_Handler& rhs) noexcept = delete;

			void WriteToFBO() const;
			void AttachFBOToTextureUnit(const GLenum& textureUnit) const;
			void ResizeFBO(const GLuint& width, const GLuint& height);

			const GLuint& GetFBOWidth() const;
			const GLuint& GetFBOHeight() const;
			const GLuint& GetFBOBuffer(const GLuint& bufferIndex) const;

			~Motion_Blur_Pass_Fbo_Handler();

		private:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}

#endif