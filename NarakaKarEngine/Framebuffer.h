#pragma once

#include <stdio.h>
#include "CommonValues.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Framebuffer
{
public:
	Framebuffer();

	virtual bool Init(GLuint width, GLuint height);

	void Write();

	virtual void Read(GLenum textureUnit);

	GLuint GetWidth() { return src_width; }

	GLuint GetHeight() { return src_height; }

	virtual ~Framebuffer() = 0;

protected:
	GLuint FBO, buffer;
	GLuint src_width, src_height;
};

