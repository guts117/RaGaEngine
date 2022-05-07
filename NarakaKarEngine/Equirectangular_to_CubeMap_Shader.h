#pragma once
#include "Shader.h"
class Equirectangular_to_CubeMap_Shader :
    public Shader
{
public:
	Equirectangular_to_CubeMap_Shader() = default;
	void SetTexture(GLuint textureUnit);
	~Equirectangular_to_CubeMap_Shader();

private:

	void CompileProgram();
};

