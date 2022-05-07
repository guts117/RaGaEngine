#ifndef DEPTH_FRAMEBUFFER
#define DEPTH_FRAMEBUFFER

#include "Framebuffer.h"
class Depth_Framebuffer :public Framebuffer
{
public:
	Depth_Framebuffer() = default;

	bool Init(GLuint width, GLuint height);


	~Depth_Framebuffer();
};
#endif