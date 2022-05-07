#pragma once
#include "Shader.h"
class HDR_Shader :
	public Shader
{
	public:
		HDR_Shader() = default;

		GLuint GetHDRLocation();
		GLuint GetExposureLocation();
		GLuint GetBlurLocation();

		void SetTexture(GLuint textureUnit);

		~HDR_Shader();

	private:
		void CompileProgram();

		GLuint uniformHDR = 0;
		GLuint uniformExposure = 0;
		GLuint uniformBlur = 0;
};


