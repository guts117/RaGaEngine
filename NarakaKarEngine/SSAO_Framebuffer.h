#pragma once
#include "Framebuffer.h"

class SSAO_Framebuffer : public Framebuffer
{
public:
	SSAO_Framebuffer() = default;

	bool Init(GLuint width, GLuint height);

	~SSAO_Framebuffer();
};

