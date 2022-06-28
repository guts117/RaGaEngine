#ifndef MODEL_SHADER
#define MODEL_SHADER

#include "Shader.h"

class Model_Shader :
	public Shader
{
public:
	Model_Shader() = default;

	void SetDirectionalLight(DirectionalLight* dLight);
	void SetPointLight(std::shared_ptr<PointLight>* pLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset);
	void SetSpotLight(std::shared_ptr<SpotLight>* sLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset);
	void SetSkybox(GLuint txtureUnit);
	void SetIrradianceMap(GLuint textureUnit);
	void SetPrefilterMap(GLuint textureUnit);
	void SetBRDFLUT(GLuint textureUnit);
	void SetDirectionalShadowMap(GLuint textureUnit);
	void SetAOMap(GLuint textureUnit);
	void SetDepthMap(GLuint textureUnit);
	void SetDirectionalLightTransform(glm::mat4 lTransform);
	void SetLightMatrices(std::vector<glm::mat4> lightMatrices);

	~Model_Shader();

private:
	void CompileProgram();
};

#endif