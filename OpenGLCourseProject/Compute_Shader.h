#pragma once
#include "Shader.h"
class Compute_Shader :
    public Shader
{
public:
	Compute_Shader() = default;
	void Dispatch(unsigned int x, unsigned int y, unsigned int z) const;
	void SetNearZ(float value);
	void SetFarZ(float value);
	~Compute_Shader();

private:
	void CompileProgram();

	GLuint uniformNearZ = -1, uniformFarZ = -1;
};
