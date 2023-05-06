#ifndef FBO_HANDLER
#define FBO_HANDLER

#include "render_pch.h"

namespace NarakaRenderEngine
{
	namespace RenderEngine
	{
		class FrameBufferObject;

		class Fbo_Handler
		{
		public:
			explicit Fbo_Handler() = delete;
			explicit Fbo_Handler(std::vector<FrameBufferObject>&& fboVector
				, const std::string& handlerName
				, const bool& isWindowSized
				, const GLuint& width
				, const GLuint& height
				, Fbo_Handler* prevHandler
				, Fbo_Handler* nextHandler);

			Fbo_Handler(Fbo_Handler&& rhs) noexcept;
			Fbo_Handler& operator=(Fbo_Handler&& rhs) noexcept;

			Fbo_Handler(const Fbo_Handler& rhs) noexcept = delete;
			Fbo_Handler& operator=(const Fbo_Handler& rhs) noexcept = delete;

			const std::string& GetHandlerName() const;
			bool CanResizeWithWindow() const;

			void BindFBO(const GLuint& fboIndex = 0) const;
			void WriteToFBOBuffer(const GLuint& fboIndex, const GLuint& texGenParamIndex, const GLuint& bufferIndex, const GLuint& faceId, const GLuint& mipLevel = 0) const;
			void AttachFBOToTextureUnit(const GLuint& fboIndex, const GLenum& textureUnit, const GLuint& texGenParamIndex, const GLuint& bufferIndex) const;
			void CreateFBOMipMap(const GLuint& fboIndex, const GLuint& texGenParamIndex, const GLuint& bufferIndex) const;
			void ResizeFBO(const GLuint& width, const GLuint& height);
			void Blit(const GLuint& fboIndex, const Fbo_Handler* to_FboHandlr, const GLuint& toFboIndex) const;

			const GLuint& GetFBOWidth(const GLuint& fboIndex = 0) const;
			const GLuint& GetFBOHeight(const GLuint& fboIndex = 0) const;
			const GLuint& GetFBOBuffer(const GLuint& fboIndex, const GLuint& bufferIndex) const;
			const GLuint& GetFBOId(const GLuint& fboIndex) const;

			~Fbo_Handler() noexcept;

		private:
			struct Impl;

			const Impl& Pimpl() const { return m_pImpl.Get(); }
			Impl& Pimpl() { return m_pImpl.Get(); }

#ifdef NDEBUG //size of string is different between debug(40) and release(32)
			ForwardDeclaredPimpl<Impl, alignof(std::string) * 11, alignof(std::string)> m_pImpl;
#else
			ForwardDeclaredPimpl<Impl, alignof(std::string) * 13, alignof(std::string)> m_pImpl;
#endif;
		};
	}
}

#endif
