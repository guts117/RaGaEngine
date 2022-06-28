#ifndef TERRAIN_PREZPASS_SHADER
#define TERRAIN_PREZPASS_SHADER

#include "PreZPass_Shader.h"

class Terrain_PreZPass_Shader :
	public PreZPass_Shader
{
public:
	Terrain_PreZPass_Shader() = default;

	~Terrain_PreZPass_Shader();

	void SetDisplacementMap(GLuint textureUnit);
	GLuint GetDispFactorLocation() { return uniformDispFactor; }


private:
	void CompileProgram();
	GLuint uniformDisplacement = 0;
	GLuint uniformDispFactor = 0;
};

#endif