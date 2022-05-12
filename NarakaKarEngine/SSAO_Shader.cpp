#include "SSAO_Shader.h"

void SSAO_Shader::CompileProgram()
{
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shaderID);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		printf("Error linking program: '%s'\n", eLog);
		return;
	}

	uniformTexture = glGetUniformLocation(shaderID, "theTexture");
	uniformNoise = glGetUniformLocation(shaderID, "noise");
	uniformProjection = glGetUniformLocation(shaderID, "Projection");
	
	uniformSampleRadius = glGetUniformLocation(shaderID, "SampleRad");

	//uniformNearPlane = glGetUniformLocation(shaderID, "nearPlane");
}

GLuint SSAO_Shader::GetSampleRadiusLocation()
{
	return uniformSampleRadius;
}

void SSAO_Shader::SetTexture(GLuint textureUnit)
{
	glUniform1i(uniformTexture, textureUnit);
}

void SSAO_Shader::SetNoiseTexture(GLuint textureUnit)
{
	glUniform1i(uniformNoise, textureUnit);
}

void SSAO_Shader::GenKernel()
{
	glm::vec3 kernel[KERNEL_SIZE];

	for (unsigned int i = 0; i < KERNEL_SIZE; i++) {
		float scale = (float)i / (float)(KERNEL_SIZE);
		glm::vec3 v;
		v.x = 2.0f * (float)rand() / RAND_MAX - 1.0f;
		v.y = 2.0f * (float)rand() / RAND_MAX - 1.0f;
		v.z = (float)rand() / RAND_MAX;
		// Use an acceleration function so more points are
		// located closer to the origin
		v *= (0.1f + 0.9f * scale * scale);

		kernel[i] = v;
	}

	glUniform3fv(uniformKernel, KERNEL_SIZE, (const GLfloat*)&kernel[0]);
}

void SSAO_Shader::GenNoise(std::vector<glm::vec3>& ssaoNoise)
{
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(
			2.0f * (float)rand() / RAND_MAX - 1.0f,
			2.0f * (float)rand() / RAND_MAX - 1.0f,
			0.0f);
		ssaoNoise[i]=noise;
	}
}

SSAO_Shader::~SSAO_Shader()
{
	ClearShader();
}

