#include "MotionBlur_Shader.h"

void MotionBlur_Shader::CompileProgram()
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
	uniformMotion = glGetUniformLocation(shaderID, "motionTexture");
	uniformVelocityScale = glGetUniformLocation(shaderID, "uVelocityScale");
}

void MotionBlur_Shader::SetTexture(GLuint textureUnit)
{
	glUniform1i(uniformTexture, textureUnit);
}

void MotionBlur_Shader::SetMotionTexture(GLuint textureUnit)
{
	glUniform1i(uniformMotion, textureUnit);
}


MotionBlur_Shader::~MotionBlur_Shader()
{
	ClearShader();
}