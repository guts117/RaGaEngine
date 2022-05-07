#include "Equirectangular_to_CubeMap_Shader.h"

void Equirectangular_to_CubeMap_Shader::CompileProgram()
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
	uniformTexture = glGetUniformLocation(shaderID, "theTexture");
}

void Equirectangular_to_CubeMap_Shader::SetTexture(GLuint textureUnit)
{
	glUniform1i(uniformTexture, textureUnit);
}

Equirectangular_to_CubeMap_Shader::~Equirectangular_to_CubeMap_Shader()
{
	ClearShader();
}

