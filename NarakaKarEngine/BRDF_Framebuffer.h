#ifndef BRDF_FRAMEBUFFER
#define BRDF_FRAMEBUFFER

#include "Framebuffer.h"

class BRDF_Framebuffer :
    public Framebuffer
{
public:
	BRDF_Framebuffer() = default;

	bool Init(GLuint width, GLuint height);
	void Write();
	void Read(GLenum textureUnit);

	~BRDF_Framebuffer();
private:
	GLuint rboDepth = 0;
};

#endif