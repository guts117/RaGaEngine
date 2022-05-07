#include "Framebuffer.h"

Framebuffer::Framebuffer()
	:FBO(0), buffer(0), src_width(0),src_height(0)
{}

bool Framebuffer::Init(GLuint width, GLuint height)
{
	return 0;
}

void Framebuffer::Write()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void Framebuffer::Read(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, buffer);
}

Framebuffer::~Framebuffer()
{

}