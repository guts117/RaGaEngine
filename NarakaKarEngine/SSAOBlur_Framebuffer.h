#ifndef	SSAOBLUR_FRAMEBUFFER
#define SSAOBLUR_FRAMEBUFFER

#include "Framebuffer.h"

class SSAOBlur_Framebuffer : public Framebuffer
{
public:
	SSAOBlur_Framebuffer() = default;

	bool Init(GLuint width, GLuint height);

	~SSAOBlur_Framebuffer();
};

#endif