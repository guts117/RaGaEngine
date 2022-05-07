#pragma once

#include <stdio.h>

#include <GL/glew.h>

class Blur_PingPong_Framebuffer
{
public:
	Blur_PingPong_Framebuffer();

	bool Init(GLuint width, GLuint height);

	void Write(bool horizontal);

	void Read(bool horizontal);
	void ReadFirstIteration(GLuint* ColorBuffer);

	GLuint GetWidth() { return src_width; }

	GLuint GetHeight() { return src_height; }

	~Blur_PingPong_Framebuffer();

protected:
	GLuint FBO[2] = {0};
	GLuint src_width, src_height;
	GLuint colorBuffers[2] = { 0 };
};

