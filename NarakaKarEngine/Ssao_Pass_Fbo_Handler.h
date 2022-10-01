#ifndef SSAO_PASS_FBO_HANDLER
#define SSAO_PASS_FBO_HANDLER

#include "pch.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Ssao_Pass_Fbo_Handler
		{
		public:
			Ssao_Pass_Fbo_Handler() = delete;
			explicit Ssao_Pass_Fbo_Handler(const GLuint& width, const GLuint& height);

			Ssao_Pass_Fbo_Handler(Ssao_Pass_Fbo_Handler&& rhs) noexcept = default;
			Ssao_Pass_Fbo_Handler& operator=(Ssao_Pass_Fbo_Handler&& rhs) noexcept = default;

			Ssao_Pass_Fbo_Handler(const Ssao_Pass_Fbo_Handler& rhs) noexcept = delete;
			Ssao_Pass_Fbo_Handler& operator=(const Ssao_Pass_Fbo_Handler& rhs) noexcept = delete;

			void WriteToFBO() const;
			void AttachFBOToTextureUnit(const GLenum& textureUnit) const;
			void ResizeFBO(const GLuint& width, const GLuint& height);

			const GLuint& GetFBOWidth() const;
			const GLuint& GetFBOHeight() const;
			const GLuint& GetFBOBuffer(const GLuint& bufferIndex) const;

			~Ssao_Pass_Fbo_Handler();

		private:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}

#endif