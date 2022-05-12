#ifndef PREFILTER_FRAMEBUFFER
#define PREFILTER_FRAMEBUFFER

#include "Framebuffer.h"
class PreFilter_Framebuffer :
    public Framebuffer
{
public:
	PreFilter_Framebuffer() = default;

	bool Init(GLuint width, GLuint height);
	void Write(int i, GLuint width, GLuint height, GLuint mip);
	void Read(GLenum textureUnit);

	~PreFilter_Framebuffer();
private:
	GLuint rboDepth = 0;
};

#endif