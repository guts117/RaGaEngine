#pragma once
#include "Shader.h"
class PreFilter_Shader :
    public Shader
{
public:
	PreFilter_Shader() = default;
	void SetSkybox(GLuint textureUnit);
	void SetRoughness(float rough);
	~PreFilter_Shader();

private:

	void CompileProgram();
	GLuint uniformRoughness = 0;
};

