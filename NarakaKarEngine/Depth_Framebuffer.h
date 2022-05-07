#pragma once

#include "Framebuffer.h"
class Depth_Framebuffer :public Framebuffer
{
public:
	Depth_Framebuffer() = default;

	bool Init(GLuint width, GLuint height);


	~Depth_Framebuffer();
};
