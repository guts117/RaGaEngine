#ifndef	SSAOBLUR_FRAMEBUFFER
#define SSAOBLUR_FRAMEBUFFER

#include "Framebuffer.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class SSAOBlur_Framebuffer : public Framebuffer
		{
		public:
			SSAOBlur_Framebuffer() = default;

			bool Init(GLuint width, GLuint height);

			void ResizeFrameBuffer(int width, int height) override;

			~SSAOBlur_Framebuffer() = default;
		};
	}
}
#endif