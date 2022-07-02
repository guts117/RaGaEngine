#ifndef BLUR_PINGPONG_FRAMEBUFFER
#define BLUR_PINGPONG_FRAMEBUFFER

#include "pch.h"
#include "Framebuffer.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Blur_PingPong_Framebuffer : public Framebuffer
		{
		public:
			Blur_PingPong_Framebuffer() = default;

			bool Init(GLuint width, GLuint height) override;
			void Write(bool horizontal);
			void Read(bool horizontal);
			void ReadFirstIteration(GLuint* ColorBuffer);
			void ResizeFrameBuffer(int width, int height) override;

			~Blur_PingPong_Framebuffer();

		protected:
			GLuint FBOs[2] = { 0 };
			GLuint colorBuffers[2] = { 0 };
		};
	}
}
#endif
