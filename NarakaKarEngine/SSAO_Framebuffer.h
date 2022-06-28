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

			~SSAO_Framebuffer();
		};
	}
}
#endif