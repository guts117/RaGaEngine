#ifndef FRAMEBUFFEROBJECT
#define FRAMEBUFFEROBJECT

#include "pch.h"
#include <variant>

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		struct FBOTexParams
		{
			GLenum ParamName;
			std::variant<GLfloat, GLint, GLfloat*, GLint*, GLuint*> Params;
		};

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

		struct FBOParams
		{
			GLuint Width;
			GLuint Height;
			std::vector<FBOTexGenParams> FboTexGenParams;
			GLenum DepthBufferFormat;
			GLenum StencilBufferFormat;
			GLenum DepthStencilBufferFormat;
		};

		class FrameBufferObject
		{
		public:
			FrameBufferObject() = delete;
			explicit FrameBufferObject(const std::shared_ptr<FBOParams>& fboParams);

			FrameBufferObject(FrameBufferObject&& rhs) noexcept = default;
			FrameBufferObject& operator=(FrameBufferObject&& rhs) noexcept = default;

			FrameBufferObject(const FrameBufferObject& rhs) noexcept = delete;
			FrameBufferObject& operator=(const FrameBufferObject& rhs) noexcept = delete;

			void WriteToBuffer() const;
			void AttachColorBufferToTexture(const GLenum& textureUnit, const GLuint& texGenParamIndex, const GLuint& bufferIndex) const;
			void ResizeBuffers(int width, int height);
			GLuint GetWidth() const;
			GLuint GetHeight() const;

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
