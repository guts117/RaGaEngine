#pragma once
#include "Framebuffer.h"
class HDR_Framebuffer : public Framebuffer
{
public:
	HDR_Framebuffer() = default;

	bool Init(GLuint width, GLuint height);

	void Read(GLenum textureUnit);

	void ReadScene(GLenum textureUnit);

	void ReadMotion(GLenum textureUnit);

	GLuint* GetColorBuffer(unsigned int i) { return &colorBuffers[i]; }
	~HDR_Framebuffer();

protected:
	GLuint rboDepth = 0;
	GLuint colorBuffers[2] = {0};
	GLuint motionBuffer = 0;
};

