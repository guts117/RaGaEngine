#include "Motion_Blur_FrameBuffer.h"

bool Motion_Blur_FrameBuffer::Init(GLuint width, GLuint height)
{
	src_width = width; src_height = height;

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1,&buffer);
	glBindTexture(GL_TEXTURE_2D, buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, src_width, src_height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer, 0);

	unsigned int attachment[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachment);

	//glGenRenderbuffers(1, &rboDepth);
	//glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, src_width, src_height);

	//// attach buffers
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebufer error: %i\n", status);
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

Motion_Blur_FrameBuffer::~Motion_Blur_FrameBuffer()
{
	if (FBO) {
		glDeleteFramebuffers(1, &FBO);
	}

	/*if(rboDepth)
	{
		glDeleteBuffers(1, &rboDepth);
	}*/

	if (buffer)
	{
		glDeleteTextures(1, &buffer);
	}
}

