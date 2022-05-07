#pragma once
#include "Framebuffer.h"
class Equirectangular_to_CubeMap_Framebuffer :
    public Framebuffer
{
public:
	Equirectangular_to_CubeMap_Framebuffer() = default;

	bool Init(GLuint width, GLuint height, bool is_env_map);
	void Write(int i);
	void Read(GLenum textureUnit);
	void CreateFirstMipFace();
	~Equirectangular_to_CubeMap_Framebuffer();
private:
	GLuint rboDepth=0;
};

