#ifndef BLOOM_PASS_FBO_HANDLER
#define BLOOM_PASS_FBO_HANDLER

#include "pch.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Bloom_Pass_Fbo_Handler
		{
		public:
			Bloom_Pass_Fbo_Handler() = delete;
			explicit Bloom_Pass_Fbo_Handler(const GLuint& width, const GLuint& height);

			Bloom_Pass_Fbo_Handler(Bloom_Pass_Fbo_Handler&& rhs) noexcept = default;
			Bloom_Pass_Fbo_Handler& operator=(Bloom_Pass_Fbo_Handler&& rhs) noexcept = default;

			Bloom_Pass_Fbo_Handler(const Bloom_Pass_Fbo_Handler& rhs) noexcept = delete;
			Bloom_Pass_Fbo_Handler& operator=(const Bloom_Pass_Fbo_Handler& rhs) noexcept = delete;

			void WriteToFBO(const GLuint& fboIndex = 0) const;
			void AttachFBOToTextureUnit(const GLenum& textureUnit, const GLuint& fboIndex = 0) const;
			void ResizeFBO(const GLuint& width, const GLuint& height);

			const GLuint& GetFBOWidth(const GLuint& fboIndex = 0) const;
			const GLuint& GetFBOHeight(const GLuint& fboIndex = 0) const;

			~Bloom_Pass_Fbo_Handler();

		private:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}

#endif

