#ifndef FRAMEBUFFEROBJECT
#define FRAMEBUFFEROBJECT

#include "render_pch.h"

namespace NarakaRenderEngine
{
	namespace RenderEngine
	{
		//ToDo: Serialize this
		struct FBORenderBufferParam
		{
			GLenum InternalFormat;
			GLenum Attachment;
		};

		//ToDo: Serialize this
		struct FBOTexParams
		{
			GLenum ParamName;
			std::variant<GLfloat, GLint, GLfloat*, GLint*, GLuint*> Params;
		};

		//ToDo: Serialize this
		struct FBOTexGenParams
		{
			GLuint ColorBufferSize;
			GLenum Target;
			GLint Level;
			GLint InternalFormat;
			GLint Border;
			GLenum Format;
			GLenum Type;
			void* PixelData;
			std::vector<FBOTexParams> FboTexParams;
		};

		//ToDo: Serialize this
		struct FBOParams
		{
			bool IsAttachBufferLater;
			GLuint Width;
			GLuint Height;
			GLenum Target;
			GLenum Attachment;
			std::vector<FBOTexGenParams> FboTexGenParams;		
			std::vector<FBORenderBufferParam> FBORenderBufferParams;
		};

		//ToDo: Serialize this
		class FrameBufferObject
		{
		public:
			explicit FrameBufferObject() = delete;
			explicit FrameBufferObject(FBOParams&& fboParams);

			FrameBufferObject(FrameBufferObject&& rhs) noexcept;
			FrameBufferObject& operator=(FrameBufferObject&& rhs) noexcept;

			FrameBufferObject(const FrameBufferObject& rhs) noexcept = delete;
			FrameBufferObject& operator=(const FrameBufferObject& rhs) noexcept = delete;

			void Bind() const;
			void WriteToBuffer(const GLuint& texParamIndex, const GLuint& bufferIndex, const GLuint& faceId = 0, const GLuint& mipLevel = 0) const;
			void AttachColorBufferToTexture(const GLenum& textureUnit, const GLuint& texGenParamIndex, const GLuint& bufferIndex) const;
			void CreateMipMap(const GLuint& texParamIndex, const GLuint& bufferIndex) const;
			void ResizeBuffers(int width, int height);
			void Blit(const GLuint& to_fboID) const;

			const GLuint& GetWidth() const;
			const GLuint& GetHeight() const;
			const GLuint& GetBuffer(const GLuint& bufferIndex) const;
			const GLuint GetFboId() const;

			~FrameBufferObject() noexcept;

		private:
			struct Impl;

			const Impl& Pimpl() const { return m_pImpl.Get(); }
			Impl& Pimpl() { return m_pImpl.Get(); }

#ifdef NDEBUG //size of vector<> is different between debug(32) and release(24)
			ForwardDeclaredPimpl<Impl, alignof(void*) * 16, alignof(void*)> m_pImpl;
#else
			ForwardDeclaredPimpl<Impl, alignof(void*) * 20, alignof(void*)> m_pImpl;
#endif;
		};
	}
}
#endif
