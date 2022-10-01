#ifndef BRDF_PASS_FBO_HANDLER
#define BRDF_PASS_FBO_HANDLER

#include "pch.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Brdf_Pass_Fbo_Handler
		{
		public:
			Brdf_Pass_Fbo_Handler() = delete;
			explicit Brdf_Pass_Fbo_Handler(const GLuint& width, const GLuint& height);

			Brdf_Pass_Fbo_Handler(Brdf_Pass_Fbo_Handler&& rhs) noexcept = default;
			Brdf_Pass_Fbo_Handler& operator=(Brdf_Pass_Fbo_Handler&& rhs) noexcept = default;

			Brdf_Pass_Fbo_Handler(const Brdf_Pass_Fbo_Handler& rhs) noexcept = delete;
			Brdf_Pass_Fbo_Handler& operator=(const Brdf_Pass_Fbo_Handler& rhs) noexcept = delete;

			void WriteToFBO() const;
			void AttachFBOToTextureUnit(const GLenum& textureUnit) const;
			void ResizeFBO(const GLuint& width, const GLuint& height);

			const GLuint& GetFBOWidth() const;
			const GLuint& GetFBOHeight() const;

			~Brdf_Pass_Fbo_Handler();

		private:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}

#endif