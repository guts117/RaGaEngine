#pragma once
#include "Shader.h"
class Particle_Shader :
	public Shader
{
public:
	Particle_Shader();

	void SetTexture(GLuint textureUnit);
	GLuint GetCameraUpLocation();
	GLuint GetCameraRightLocation();

	~Particle_Shader();

private:
	void CompileProgram();

	GLuint uniformCameraRight= 0;
	GLuint uniformCameraUp = 0;
	GLuint uniformPos = 0;
	GLuint uniformSize = 0;

};

