#pragma once
#include "Shader.h"
class Compute_Shader :
    public Shader
{
public:
	Compute_Shader() = default;
	void Dispatch(unsigned int x, unsigned int y, unsigned int z) const;
	~Compute_Shader();

private:
	void CompileProgram();
};
