#pragma once
#include "ShadowMap_Framebuffer.h"
class OmniShadowMap_Framebuffer :
	public ShadowMap_Framebuffer
{
public:
	OmniShadowMap_Framebuffer()= default;

	bool Init(GLuint width, GLuint height);

	void Read(int i, GLenum textureUnit);

	~OmniShadowMap_Framebuffer();

};
