#ifndef	SSAO_FRAMEBUFFER
#define SSAO_FRAMEBUFFER

#include "Framebuffer.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class SSAO_Framebuffer : public Framebuffer
		{
		public:
			SSAO_Framebuffer() = default;

			bool Init(GLuint width, GLuint height);

			void ResizeFrameBuffer(int width, int height) override;

			~SSAO_Framebuffer() = default;
		};
	}
}
#endif