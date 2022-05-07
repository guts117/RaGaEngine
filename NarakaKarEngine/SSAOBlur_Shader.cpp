#include "SSAOBlur_Shader.h"

void SSAOBlur_Shader::CompileProgram()
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

	uniformTexture = glGetUniformLocation(shaderID, "theTexture");
}

void SSAOBlur_Shader::SetTexture(GLuint textureUnit)
{
	glUniform1i(uniformTexture, textureUnit);
}

SSAOBlur_Shader::~SSAOBlur_Shader()
{
	ClearShader();
}
