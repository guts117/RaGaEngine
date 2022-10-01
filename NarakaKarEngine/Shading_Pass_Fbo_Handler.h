#ifndef SHADING_PASS_FBO_HANDLER
#define SHADING_PASS_FBO_HANDLER

#include "Framebuffer.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		enum ShadingPassBufferType
		{
			Scene,
			SceneExposed,
			Motion
		};

		class Shading_Pass_Fbo_Handler
		{
		public:
			Shading_Pass_Fbo_Handler() = delete;
			explicit Shading_Pass_Fbo_Handler(const GLuint& width, const GLuint& height);

			Shading_Pass_Fbo_Handler(Shading_Pass_Fbo_Handler&& rhs) noexcept = default;
			Shading_Pass_Fbo_Handler& operator=(Shading_Pass_Fbo_Handler&& rhs) noexcept = default;

			Shading_Pass_Fbo_Handler(const Shading_Pass_Fbo_Handler& rhs) noexcept = delete;
			Shading_Pass_Fbo_Handler& operator=(const Shading_Pass_Fbo_Handler& rhs) noexcept = delete;

			void WriteToFBO() const;
			void AttachFBOToTextureUnit(const GLenum& textureUnit, const GLuint& bufferIndex) const;
			void ResizeFBO(const GLuint& width, const GLuint& height);

			const GLuint& GetFBOWidth() const;
			const GLuint& GetFBOHeight() const;
			const GLuint& GetFBOBuffer(const GLuint& bufferIndex) const;

			~Shading_Pass_Fbo_Handler();

		private:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}
#endif