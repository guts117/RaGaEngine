#ifndef	OMNISHADOWMAP_FRAMEBUFFER
#define OMNISHADOWMAP_FRAMEBUFFER

#include "ShadowMap_FrameBuffer.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class OmniShadowMap_Framebuffer :
			public ShadowMap_Framebuffer
		{
		public:
			OmniShadowMap_Framebuffer() = default;

			bool Init(GLuint width, GLuint height);

			void Read(int i, GLenum textureUnit);

			~OmniShadowMap_Framebuffer();

		};
	}
}
#endif