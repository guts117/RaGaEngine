#pragma once
#include "Shader.h"
class MotionBlur_Shader :
	public Shader
{
public:
	MotionBlur_Shader() = default;

	void SetTexture(GLuint textureUnit);
	void SetMotionTexture(GLuint textureUnit);
	
	GLuint GetVelocityScaleLocation() { return uniformVelocityScale; }

	~MotionBlur_Shader();

private:
	void CompileProgram();

	GLuint uniformMotion = 0;
	GLuint uniformVelocityScale = 0;
};

