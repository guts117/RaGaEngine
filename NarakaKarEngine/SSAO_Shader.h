#ifndef	SSAO_SHADER
#define SSAO_SHADER

#include "Shader.h"

class SSAO_Shader :
	public Shader
{
public:
	SSAO_Shader() = default;

	void SetTexture(GLuint textureUnit);
	void SetNoiseTexture(GLuint textureUnit);
	void  GenKernel();
	void  GenNoise(std::vector<glm::vec3>& ssaoNoise);
	~SSAO_Shader();

	GLuint GetSampleRadiusLocation();

private:
	void CompileProgram();

	GLuint uniformNoise = 0;
	GLuint uniformKernel = 0;
	GLuint uniformSampleRadius = 0;
	const static unsigned int KERNEL_SIZE = 64;

	float lerp(float a, float b, float f)
	{
		return a + f * (b - a);
	}
};

#endif