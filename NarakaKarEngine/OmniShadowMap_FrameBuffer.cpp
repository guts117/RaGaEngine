#include "OmniShadowMap_FrameBuffer.h"

bool OmniShadowMap_Framebuffer::Init(GLuint width, GLuint height)
{
	src_width = width; src_height = height;

	glGenFramebuffers(1, &FBO);

	glGenTextures(1, &buffer);
	glBindTexture(GL_TEXTURE_CUBE_MAP, buffer);

	for (size_t i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, src_width, src_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, buffer, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebufer error: %i\n", status);
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}


void OmniShadowMap_Framebuffer::Read(int i, GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, buffer);
}

OmniShadowMap_Framebuffer::~OmniShadowMap_Framebuffer()
{
	if (FBO) {
		glDeleteFramebuffers(1, &FBO);
	}
	if (buffer) {
		glDeleteTextures(1, &buffer);
	}
}
