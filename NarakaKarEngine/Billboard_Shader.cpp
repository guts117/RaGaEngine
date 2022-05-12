#include "Billboard_Shader.h"

void Billboard_Shader::CompileProgram()
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
	uniformCameraUp = glGetUniformLocation(shaderID, "CameraUp_worldspace");
	uniformCameraRight = glGetUniformLocation(shaderID, "CameraRight_worldspace");
	uniformPos = glGetUniformLocation(shaderID, "BillboardPos");
	uniformSize = glGetUniformLocation(shaderID, "BillboardSize");
	uniformPrevPVM = glGetUniformLocation(shaderID, "prevPV");

	uniformTexture = glGetUniformLocation(shaderID, "theTexture");
	
}

void Billboard_Shader::SetTexture(GLuint textureUnit)
{
	glUniform1i(uniformTexture, textureUnit);
}

GLuint Billboard_Shader::GetCameraUpLocation()
{
	return uniformCameraUp;
}

GLuint Billboard_Shader::GetCameraRightLocation()
{
	return uniformCameraRight;
}

GLuint Billboard_Shader::GetPosLocation()
{
	return uniformPos;
}

GLuint Billboard_Shader::GetSizeLocation()
{
	return uniformSize;
}

Billboard_Shader::~Billboard_Shader()
{
	ClearShader();
}