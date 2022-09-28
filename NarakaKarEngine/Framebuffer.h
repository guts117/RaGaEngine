#ifndef FRAMEBUFFER
#define FRAMEBUFFER

#include "pch.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Framebuffer
		{
		public:
			Framebuffer();

			virtual bool Init(GLuint width, GLuint height);

			void Write();

			virtual void Read(GLenum textureUnit);

			virtual void ResizeFrameBuffer(int width, int height);

			GLuint GetWidth() { return src_width; }

			GLuint GetHeight() { return src_height; }

			GLuint GetBuffer() { return buffer; }

			virtual ~Framebuffer() = 0;

		protected:
			GLuint FBO, buffer;
			GLuint src_width, src_height;
		};
	}
}
#endif