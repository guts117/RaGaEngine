#pragma once
#include "Shader.h"
class Irradiance_Convolution_Shader :
    public Shader
{
public:
	Irradiance_Convolution_Shader() = default;
	void SetSkybox(GLuint textureUnit);
	~Irradiance_Convolution_Shader();

private:

	void CompileProgram();
};

