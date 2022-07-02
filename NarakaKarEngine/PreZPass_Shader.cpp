#include "pch.h"
#include "PreZPass_Shader.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

void PreZPass_Shader::CompileProgram()
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
}

PreZPass_Shader::~PreZPass_Shader()
{
	ClearShader();
}
