#pragma once
#include "Shader.h"
class SSAOBlur_Shader :
	public Shader
{
public:
	SSAOBlur_Shader() = default;

	void SetTexture(GLuint textureUnit);
	~SSAOBlur_Shader();


private:
	void CompileProgram();
};

