#include "PreFilter_Shader.h"

void PreFilter_Shader::CompileProgram()
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
	uniformProjection = glGetUniformLocation(shaderID, "Projection");
	uniformView = glGetUniformLocation(shaderID, "View");
	uniformSkybox = glGetUniformLocation(shaderID, "skybox");
	uniformRoughness = glGetUniformLocation(shaderID, "roughness");
}

void PreFilter_Shader::SetSkybox(GLuint textureUnit)
{
	glUniform1i(uniformSkybox, textureUnit);
}

void PreFilter_Shader::SetRoughness(float rough)
{
	glUniform1f(uniformRoughness, rough);
}

PreFilter_Shader::~PreFilter_Shader()
{
	ClearShader();
}