#pragma once
#include "Shader.h"
class PreZPass_Shader :
	public Shader
{
	public:
		PreZPass_Shader() = default;

		~PreZPass_Shader();

	private:
		void CompileProgram();
};

