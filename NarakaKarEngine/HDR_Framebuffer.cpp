#include "HDR_Framebuffer.h"

bool HDR_Framebuffer::Init(GLuint width, GLuint height)
{
	src_width = width; src_height = height;

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(2, colorBuffers);
	glGenTextures(1, &motionBuffer);

	for (unsigned int i = 0; i < 3; ++i)
	{
		if (i < 2)
		{
			glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, src_width, src_height, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, motionBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, src_width, src_height, 0, GL_RG, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, motionBuffer, 0);
		}
		// attach buffers
		
	}

	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, src_width,  src_height);

	// attach buffers
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebufer error: %i\n", status);
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}



void HDR_Framebuffer::Read(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, colorBuffers[1]);
}

void HDR_Framebuffer::ReadScene(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
}

void HDR_Framebuffer::ReadMotion(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, motionBuffer);
}

HDR_Framebuffer::~HDR_Framebuffer()
{
	if (FBO) {
		glDeleteFramebuffers(1, &FBO);
	}
	if (rboDepth) {
		glDeleteBuffers(1, &rboDepth);
	}
	if (colorBuffers) 
	{
		glDeleteTextures(2, colorBuffers);
	}
	if (motionBuffer)
	{
		glDeleteTextures(1, &motionBuffer);
	}
}