#ifndef FRAMEBUFFEROBJECT
#define FRAMEBUFFEROBJECT

#include "pch.h"
#include <variant>

namespace NarakaKarEngine
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
			FrameBufferObject() = delete;
			explicit FrameBufferObject(std::shared_ptr<FBOParams> fboParams);

			FrameBufferObject(FrameBufferObject&& rhs) noexcept = default;
			FrameBufferObject& operator=(FrameBufferObject&& rhs) noexcept = default;

			FrameBufferObject(const FrameBufferObject& rhs) noexcept = delete;
			FrameBufferObject& operator=(const FrameBufferObject& rhs) noexcept = delete;

			void Bind() const;
			void WriteToBuffer(const GLuint& texParamIndex, const GLuint& bufferIndex, const GLuint& faceId = 0, const GLuint& mipLevel = 0) const;
			void AttachColorBufferToTexture(const GLenum& textureUnit, const GLuint& texGenParamIndex, const GLuint& bufferIndex) const;
			void CreateMipMap(const GLuint& texParamIndex, const GLuint& bufferIndex) const;
			void ResizeBuffers(int width, int height);
			const GLuint& GetWidth() const;
			const GLuint& GetHeight() const;
			const GLuint& GetBuffer(const GLuint& bufferIndex) const;

			~FrameBufferObject();

		private:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}
#endif
