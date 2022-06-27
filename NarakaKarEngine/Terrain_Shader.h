#ifndef TERRAIN_SHADER
#define TERRAIN_SHADER

#include "Shader.h"
#include "CommonValues.h"

class Light;

class Terrain_Shader :
	public Shader
{
public:
	Terrain_Shader() = default;

	GLuint GetDispFactorLocation();

	GLuint GetDisplacementLocation();

	void SetDirectionalLight(DirectionalLight* dLight);
	void SetPointLight(std::shared_ptr<PointLight>* pLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset);
	void SetSpotLight(std::shared_ptr<SpotLight>* sLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset);
	void SetDirectionalShadowMaps(Light* light, unsigned int i,GLuint textureUnit);
	void SetBlendMap(GLuint textureUnit);
	void SetDisplacementMap(GLuint textureUnit);
	void SetAOMap(GLuint textureUnit);
	void SetDepthMap(GLuint textureUnit);
	void SetIrradianceMap(GLuint textureUnit);
	void SetPrefilterMap(GLuint textureUnit);
	void SetBRDFLUT(GLuint textureUnit);
	void SetDirectionalLightTransforms(int i, glm::mat4* lTransform);
	void SetDirectionalLightTransform(glm::mat4* lTransform);
	void SetCascadeEndClipSpace(int i, float z);
	void SetLightMatrices(std::vector<glm::mat4> lightMatrices);

	~Terrain_Shader();
private:
	void CompileProgram();
	GLuint uniformBlend = 0;
	GLuint uniformDispFactor = 0;
	GLuint uniformDisplacement = 0;
	GLuint uniformDirectionalLightTransforms[NUM_CASCADES] = { 0 };
	GLuint uniformCascadeEndClipSpace[NUM_CASCADES] = { 0 };
	struct {
		GLuint shadowMap;
	} uniformDirectionalShadowMaps[NUM_CASCADES];
};

#endif