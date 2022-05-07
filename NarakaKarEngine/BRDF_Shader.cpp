#include "BRDF_Shader.h"

void BRDF_Shader::CompileProgram()
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
}

BRDF_Shader::~BRDF_Shader()
{
	ClearShader();
}