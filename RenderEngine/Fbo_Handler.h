#ifndef FBO_HANDLER
#define FBO_HANDLER

#include "render_pch.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class FrameBufferObject;

		class Fbo_Handler
		{
		public:
			Fbo_Handler() = delete;
			explicit Fbo_Handler(std::unique_ptr<std::vector<std::shared_ptr<FrameBufferObject>>>&& fboVectorPtr
				, const std::string& handlerName
				, const bool& isWindowSized
				, const GLuint& width
				, const GLuint& height
				, Fbo_Handler* prevHandler
				, Fbo_Handler* nextHandler);

			Fbo_Handler(Fbo_Handler&& rhs) noexcept = default;
			Fbo_Handler& operator=(Fbo_Handler&& rhs) noexcept = default;

			Fbo_Handler(const Fbo_Handler& rhs) noexcept = delete;
			Fbo_Handler& operator=(const Fbo_Handler& rhs) noexcept = delete;

			const std::string& GetHandlerName() const;
			bool CanResizeWithWindow() const;

			void BindFBO(const GLuint& fboIndex = 0) const;
			void WriteToFBOBuffer(const GLuint& fboIndex, const GLuint& texGenParamIndex, const GLuint& bufferIndex, const GLuint& faceId, const GLuint& mipLevel = 0) const;
			void AttachFBOToTextureUnit(const GLuint& fboIndex, const GLenum& textureUnit, const GLuint& texGenParamIndex, const GLuint& bufferIndex) const;
			void CreateFBOMipMap(const GLuint& fboIndex, const GLuint& texGenParamIndex, const GLuint& bufferIndex) const;
			void ResizeFBO(const GLuint& width, const GLuint& height);
			void Blit(const GLuint& fboIndex, const Fbo_Handler& to_FboHandlr, const GLuint& toFboIndex) const;

			const GLuint& GetFBOWidth(const GLuint& fboIndex = 0) const;
			const GLuint& GetFBOHeight(const GLuint& fboIndex = 0) const;
			const GLuint& GetFBOBuffer(const GLuint& fboIndex, const GLuint& bufferIndex) const;
			const GLuint& GetFBOId(const GLuint& fboIndex) const;

			~Fbo_Handler();

		private:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}

#endif
