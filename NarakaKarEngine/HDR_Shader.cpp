#include "HDR_Shader.h"


void HDR_Shader::CompileProgram()
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

	uniformHDR = glGetUniformLocation(shaderID, "hdr");
	uniformExposure = glGetUniformLocation(shaderID, "exposure");
	uniformTexture = glGetUniformLocation(shaderID, "theTexture");
	uniformBlur = glGetUniformLocation(shaderID, "blurTexture");
}

void HDR_Shader::SetTexture(GLuint textureUnit)
{
	glUniform1i(uniformTexture, textureUnit);
}


GLuint HDR_Shader::GetHDRLocation()
{
	return uniformHDR;
}

GLuint HDR_Shader::GetExposureLocation()
{
	return uniformExposure;
}

GLuint HDR_Shader::GetBlurLocation()
{
	return uniformBlur;
}


HDR_Shader::~HDR_Shader()
{
	ClearShader();
}