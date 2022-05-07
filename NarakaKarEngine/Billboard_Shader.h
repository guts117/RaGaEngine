#pragma once
#include "Shader.h"
class Billboard_Shader :
	public Shader
{
public:
	Billboard_Shader() = default;

	GLuint GetCameraUpLocation();
	GLuint GetCameraRightLocation();
	GLuint GetPosLocation();
	GLuint GetSizeLocation();
	void SetTexture(GLuint textureUnit);
	~Billboard_Shader();

private:

	void CompileProgram();
	GLuint uniformCameraRight = 0;
	GLuint uniformCameraUp = 0;
	GLuint uniformPos = 0;
	GLuint uniformSize = 0;
};

