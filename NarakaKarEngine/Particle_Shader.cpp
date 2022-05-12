#include "Particle_Shader.h"

Particle_Shader::Particle_Shader()
{
}


void Particle_Shader::CompileProgram()
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

	uniformTexture = glGetUniformLocation(shaderID, "theTexture");

}

void Particle_Shader::SetTexture(GLuint textureUnit)
{
	glUniform1i(uniformTexture, textureUnit);
}

GLuint Particle_Shader::GetCameraUpLocation()
{
	return uniformCameraUp;
}

GLuint Particle_Shader::GetCameraRightLocation()
{
	return uniformCameraRight;
}


Particle_Shader::~Particle_Shader()
{
	ClearShader();
}