#include "SSAO_Framebuffer.h"

bool SSAO_Framebuffer::Init(GLuint width, GLuint height)
{
	src_width = width; src_height = height;

	glGenFramebuffers(1, &FBO);

	glGenTextures(1, &buffer);
	glBindTexture(GL_TEXTURE_2D, buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, src_width, src_height, 0, GL_RED, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer, 0);

	unsigned int attachment[1] = { GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, attachment);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebufer error: %i\n", status);
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

SSAO_Framebuffer::~SSAO_Framebuffer()
{
	if (FBO) {
		glDeleteFramebuffers(1, &FBO);
	}

	if (buffer)
	{
		glDeleteTextures(1, &buffer);
	}
}