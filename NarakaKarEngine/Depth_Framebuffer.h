#ifndef DEPTH_FRAMEBUFFER
#define DEPTH_FRAMEBUFFER

#include "Framebuffer.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Depth_Framebuffer :public Framebuffer
		{
		public:
			Depth_Framebuffer() = default;

			bool Init(GLuint width, GLuint height);
			
			void ResizeFrameBuffer(int width, int height) override;

			~Depth_Framebuffer() = default;
		};
	}
}
#endif