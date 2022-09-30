#ifndef BRDF_PASS_FBO_HANDLER
#define BRDF_PASS_FBO_HANDLER

#include "pch.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class BRDF_Pass_FBO_Handler
		{
		public:
			BRDF_Pass_FBO_Handler() = delete;
			explicit BRDF_Pass_FBO_Handler(const GLuint& width, const GLuint& height);

			BRDF_Pass_FBO_Handler(BRDF_Pass_FBO_Handler&& rhs) noexcept = default;
			BRDF_Pass_FBO_Handler& operator=(BRDF_Pass_FBO_Handler&& rhs) noexcept = default;

			BRDF_Pass_FBO_Handler(const BRDF_Pass_FBO_Handler& rhs) noexcept = delete;
			BRDF_Pass_FBO_Handler& operator=(const BRDF_Pass_FBO_Handler& rhs) noexcept = delete;

			void WriteToFBO() const;
			void AttachFBOToTextureUnit(const GLenum& textureUnit) const;
			void ResizeFBO(const GLuint& width, const GLuint& height);

			const GLuint& GetFBOWidth() const;
			const GLuint& GetFBOHeight() const;

			~BRDF_Pass_FBO_Handler();

		private:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}

#endif