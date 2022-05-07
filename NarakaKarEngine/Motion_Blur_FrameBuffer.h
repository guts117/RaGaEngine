#pragma once
#include "Framebuffer.h"
class Motion_Blur_FrameBuffer :
	public Framebuffer
{
public:
	Motion_Blur_FrameBuffer() = default;

	bool Init(GLuint width, GLuint height);

	~Motion_Blur_FrameBuffer();
private:
	//GLuint rboDepth = 0;
};

