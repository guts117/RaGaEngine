#include "pch.h"
#include "Terrain_Shader.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Fbo_Handler.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

void Terrain_Shader::CompileProgram()
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

	uniformModel = glGetUniformLocation(shaderID, "Model");
	uniformProjection = glGetUniformLocation(shaderID, "Projection");
	uniformView = glGetUniformLocation(shaderID, "View");
	uniformPrevPVM = glGetUniformLocation(shaderID, "prevPVM");
	uniformDirectionalLight.uniformColor = glGetUniformLocation(shaderID, "directionalLight.color");
	uniformDirectionalLight.uniformDirection = glGetUniformLocation(shaderID, "directionalLight.direction");
	uniformEyePosition = glGetUniformLocation(shaderID, "eyePosition");
	uniformDispFactor = glGetUniformLocation(shaderID, "dispFactor");
	uniformHeightScale = glGetUniformLocation(shaderID, "height_scale");
	uniformBlend = glGetUniformLocation(shaderID, "blendMap");
	uniformAlbedo = glGetUniformLocation(shaderID, "material.albedoMap");
	uniformMetallic = glGetUniformLocation(shaderID, "material.metallicMap");
	uniformNormal = glGetUniformLocation(shaderID, "material.normalMap");
	uniformParallax = glGetUniformLocation(shaderID, "material.parallaxMap");
	uniformDisplacement = glGetUniformLocation(shaderID, "displacementMap");
	uniformRoughness = glGetUniformLocation(shaderID, "material.roughnessMap");

	uniformSpotLightCount = glGetUniformLocation(shaderID, "SpotLightCount");

	for (size_t i = 0; i < MAX_SPOT_LIGHTS; i++) {

		char locBuff[100] = { '\0' };

		snprintf(locBuff, sizeof(locBuff), "spotLights[%zd].color", i);
		uniformSpotLight[i].uniformColor = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%zd].position", i);
		uniformSpotLight[i].uniformPosition = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%zd].direction", i);
		uniformSpotLight[i].uniformDirection = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%zd].edge", i);
		uniformSpotLight[i].uniformEdge = glGetUniformLocation(shaderID, locBuff);
	}
	uniformAO = glGetUniformLocation(shaderID, "AOMap");
	uniformDepth = glGetUniformLocation(shaderID, "depthMap");
	uniformIrradiance = glGetUniformLocation(shaderID, "irradianceMap");
	uniformPrefilter = glGetUniformLocation(shaderID, "prefilterMap");
	uniformBRDF = glGetUniformLocation(shaderID, "brdfLUT");
	uniformDirectionalLightTransform = glGetUniformLocation(shaderID, "DirectionalLightTransform");
	uniformOmniLightPos = glGetUniformLocation(shaderID, "lightPos");

	for (size_t i = 0; i < NUM_CASCADES; i++) {
		char locBuff[100] = { '\0' };

		snprintf(locBuff, sizeof(locBuff), "DirectionalLightTransforms[%zd]", i);
		uniformDirectionalLightTransforms[i] = glGetUniformLocation(shaderID, locBuff);
	}

	for (size_t i = 0; i < NUM_CASCADES; i++) {
		char locBuff[100] = { '\0' };

		snprintf(locBuff, sizeof(locBuff), "directionalShadowMaps[%zd].shadowMap", i);
		uniformDirectionalShadowMaps[i].shadowMap = glGetUniformLocation(shaderID, locBuff);
	}

	for (size_t i = 0; i < NUM_CASCADES; i++) {
		char locBuff[100] = { '\0' };

		snprintf(locBuff, sizeof(locBuff), "CascadeEndClipSpace[%zd]", i);
		uniformCascadeEndClipSpace[i] = glGetUniformLocation(shaderID, locBuff);
	}

	for (size_t i = 0; i < 6; i++) {
		char locBuff[100] = { '\0' };

		snprintf(locBuff, sizeof(locBuff), "lightMatrices[%zd]", i);
		uniformLightMatrices[i] = glGetUniformLocation(shaderID, locBuff);
	}
	//omnishadowmap
	for (size_t i = 0; i < MAX_POINT_LIGHTS_WITH_SHADOW + MAX_SPOT_LIGHTS; i++) {
		char locBuff[100] = { '\0' };

		snprintf(locBuff, sizeof(locBuff), "omniShadowMaps[%zd].shadowMap", i);
		uniformOmniShadowMap[i].shadowMap = glGetUniformLocation(shaderID, locBuff);
	}

	for (size_t i = 0; i < MAX_POINT_LIGHTS_WITH_SHADOW + MAX_SPOT_LIGHTS; i++) {
		char locBuff[100] = { '\0' };

		snprintf(locBuff, sizeof(locBuff), "omniShadowMaps[%zd].farPlane", i);
		uniformOmniShadowMap[i].farPlane = glGetUniformLocation(shaderID, locBuff);
	}
}

GLuint Terrain_Shader::GetDispFactorLocation()
{
	return uniformDispFactor;
}

GLuint Terrain_Shader::GetDisplacementLocation()
{
	return uniformDisplacement;
}


void Terrain_Shader::SetDirectionalLight(DirectionalLight* dLight) {

	dLight->UseLight(uniformDirectionalLight.uniformColor, uniformDirectionalLight.uniformDirection);
}

void Terrain_Shader::SetPointLight(std::shared_ptr<PointLight>* pLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset) {

	if (lightCount > MAX_POINT_LIGHTS_WITH_SHADOW) lightCount = MAX_POINT_LIGHTS_WITH_SHADOW;

	for (size_t i = 0; i < lightCount; i++) {																									//pLight is already a pointer 
		pLight[i]->GetShadowMap()->AttachFBOToTextureUnit(0, GL_TEXTURE0 + textureUnit + i, 0, 0);
		glUniform1i(uniformOmniShadowMap[i + offset].shadowMap, textureUnit + i);
		glUniform1f(uniformOmniShadowMap[i + offset].farPlane, pLight[i]->GetFarPlane());
	}
}

void Terrain_Shader::SetSpotLight(std::shared_ptr<SpotLight>* sLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset) {

	if (lightCount > MAX_POINT_LIGHTS_WITH_SHADOW) lightCount = MAX_SPOT_LIGHTS;

	glUniform1i(uniformSpotLightCount, lightCount);

	for (size_t i = 0; i < lightCount; i++) {																									//sLight is already a pointer 
		sLight[i]->UseLight(uniformSpotLight[i].uniformColor,									//(sLight+i) = sLight[i]  //*(a+1) = a[i]
			uniformSpotLight[i].uniformPosition, uniformSpotLight[i].uniformDirection,
			uniformSpotLight[i].uniformEdge);

		sLight[i]->GetShadowMap()->AttachFBOToTextureUnit(0, GL_TEXTURE0 + textureUnit + i, 0, 0);
		glUniform1i(uniformOmniShadowMap[i + offset].shadowMap, textureUnit + i);
		glUniform1f(uniformOmniShadowMap[i + offset].farPlane, sLight[i]->GetFarPlane());
	}
}

void Terrain_Shader::SetDirectionalShadowMaps(DirectionalLight* light, unsigned int i, GLuint textureUnit)
{
	for (size_t j = 0; j < i; ++j) 
	{
		light->GetShadowMap()->AttachFBOToTextureUnit(0, GL_TEXTURE2 + j, 0, j);
		glUniform1i(uniformDirectionalShadowMaps[j].shadowMap, textureUnit + j);
	}
}

void Terrain_Shader::SetBlendMap(GLuint textureUnit)
{
	glUniform1i(uniformBlend, textureUnit);
}

void Terrain_Shader::SetDisplacementMap(GLuint textureUnit)
{
	glUniform1i(uniformDisplacement, textureUnit);
}

void Terrain_Shader::SetAOMap(GLuint textureUnit)
{
	glUniform1i(uniformAO, textureUnit);
}

void Terrain_Shader::SetDepthMap(GLuint textureUnit)
{
	glUniform1i(uniformDepth, textureUnit);
}

void Terrain_Shader::SetIrradianceMap(GLuint textureUnit)
{
	glUniform1i(uniformIrradiance, textureUnit);
}

void Terrain_Shader::SetPrefilterMap(GLuint textureUnit)
{
	glUniform1i(uniformPrefilter, textureUnit);
}

void Terrain_Shader::SetBRDFLUT(GLuint textureUnit)
{
	glUniform1i(uniformBRDF, textureUnit);
}

void Terrain_Shader::SetDirectionalLightTransforms(int i, glm::mat4* lTransform)
{
	glUniformMatrix4fv(uniformDirectionalLightTransforms[i], 1, GL_FALSE, glm::value_ptr(*lTransform));
}

void Terrain_Shader::SetDirectionalLightTransform(glm::mat4* lTransform)
{
	glUniformMatrix4fv(uniformDirectionalLightTransform, 1, GL_FALSE, glm::value_ptr(*lTransform));
}


void Terrain_Shader::SetCascadeEndClipSpace(int i, float z)
{
	glUniform1f(uniformCascadeEndClipSpace[i], z);
}

void Terrain_Shader::SetLightMatrices(std::vector<glm::mat4> lightMatrices)
{
	for (size_t i = 0; i < 6; i++) {

		glUniformMatrix4fv(uniformLightMatrices[i], 1, GL_FALSE, glm::value_ptr(lightMatrices[i]));
	}
}

Terrain_Shader::~Terrain_Shader()
{
	ClearShader();
}
