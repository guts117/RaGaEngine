#include "pch.h"
#include "Blur_PingPong_Framebuffer.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;


bool Blur_PingPong_Framebuffer::Init(GLuint width, GLuint height)
{
	src_width = width; src_height = height;

	glGenFramebuffers(2, FBOs);
	glGenTextures(2, colorBuffers);

	for (unsigned int i = 0; i < 2; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FBOs[i]);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, src_width, src_height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// attach buffers
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffers[i], 0);
	}

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebufer error: %i\n", status);
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void Blur_PingPong_Framebuffer::Write(bool horizontal)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBOs[horizontal]);
}

void Blur_PingPong_Framebuffer::Read(bool horizontal)
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, colorBuffers[horizontal]);
}

void Blur_PingPong_Framebuffer::ReadFirstIteration(GLuint* ColorBuffer)
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, *ColorBuffer);
}

void Blur_PingPong_Framebuffer::ResizeFrameBuffer(int width, int height)
{
	src_width = width;
	src_height = height;
	for (unsigned int i = 0; i < 2; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, src_width, src_height, 0, GL_RGB, GL_FLOAT, NULL);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

Blur_PingPong_Framebuffer::~Blur_PingPong_Framebuffer()
{

	if (FBOs) {
		glDeleteFramebuffers(2, FBOs);
	}

	if (colorBuffers) {
		glDeleteTextures(2, colorBuffers);
	}

}
