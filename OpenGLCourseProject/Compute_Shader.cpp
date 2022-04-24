#include "Compute_Shader.h"

void Compute_Shader::CompileProgram()
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

	uniformNearZ = glGetUniformLocation(shaderID, "zNear");
	uniformFarZ = glGetUniformLocation(shaderID, "zFar");
	uniformView = glGetUniformLocation(shaderID, "View");
}

void Compute_Shader::Dispatch(unsigned int x, unsigned int y, unsigned int z) const
{
	glDispatchCompute(x, y, z);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void Compute_Shader::SetNearZ(float value) 
{
	glUniform1f(uniformNearZ, value);
}

void Compute_Shader::SetFarZ(float value)
{
	glUniform1f(uniformFarZ, value);
}

Compute_Shader::~Compute_Shader()
{
	ClearShader();
}
