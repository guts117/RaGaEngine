#ifndef MOTIONBLUR_FRAMEBUFFER
#define MOTIONBLUR_FRAMEBUFFER

#include "Framebuffer.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class MotionBlur_FrameBuffer :
			public Framebuffer
		{
		public:
			MotionBlur_FrameBuffer() = default;

			bool Init(GLuint width, GLuint height);

			~MotionBlur_FrameBuffer();
		private:
			//GLuint rboDepth = 0;
		};
	}
}
#endif