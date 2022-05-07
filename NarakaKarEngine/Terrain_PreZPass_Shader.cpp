#include "Terrain_PreZPass_Shader.h"

void Terrain_PreZPass_Shader::CompileProgram()
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
	uniformModel = glGetUniformLocation(shaderID, "Model");
	uniformProjection = glGetUniformLocation(shaderID, "Projection");
	uniformView = glGetUniformLocation(shaderID, "View");
	uniformEyePosition = glGetUniformLocation(shaderID, "eyePosition");
	uniformDisplacement = glGetUniformLocation(shaderID, "displacementMap");
	uniformDispFactor = glGetUniformLocation(shaderID, "dispFactor");
}

void Terrain_PreZPass_Shader::SetDisplacementMap(GLuint textureUnit)
{
	glUniform1i(uniformDisplacement, textureUnit);
}


Terrain_PreZPass_Shader::~Terrain_PreZPass_Shader()
{
	ClearShader();
}
