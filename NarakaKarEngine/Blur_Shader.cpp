#include "Blur_Shader.h"

Blur_Shader::Blur_Shader()
{

}

void Blur_Shader::CompileProgram()
{
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shaderID);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		printf("Error linking program: '%s'\n", eLog);
		return;
	}

	uniformHorizontal = glGetUniformLocation(shaderID, "horizontal");
	uniformWeight = glGetUniformLocation(shaderID, "weight");
	uniformTexture = glGetUniformLocation(shaderID, "theTexture");
}

void Blur_Shader::SetTexture(GLuint textureUnit)
{
	glUniform1i(uniformTexture, textureUnit);
}

GLuint Blur_Shader::GetHorizontalLocation()
{
	return uniformHorizontal;
}

GLuint Blur_Shader::GetWeightLocation()
{
	return uniformWeight;
}

Blur_Shader::~Blur_Shader()
{
	ClearShader();
}