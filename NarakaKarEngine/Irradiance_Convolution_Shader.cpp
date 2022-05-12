#include "Irradiance_Convolution_Shader.h"

void Irradiance_Convolution_Shader::CompileProgram()
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
}

void Irradiance_Convolution_Shader::SetSkybox(GLuint textureUnit)
{
	glUniform1i(uniformSkybox, textureUnit);
}

Irradiance_Convolution_Shader::~Irradiance_Convolution_Shader()
{
	ClearShader();
}