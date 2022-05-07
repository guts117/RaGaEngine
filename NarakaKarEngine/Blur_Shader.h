#pragma once
#include "Shader.h"
class Blur_Shader :
	public Shader
{
public:
	Blur_Shader();

	void SetTexture(GLuint textureUnit);
	GLuint GetHorizontalLocation();
	GLuint GetWeightLocation();

	~Blur_Shader();

private:
	void CompileProgram();
	GLuint uniformHorizontal = 0;
	GLuint uniformWeight = 0;
};

