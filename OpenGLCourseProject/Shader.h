#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Animated_Model.h"

#include "CommonValues.h"

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class Shader
{
public:
	Shader() = default;

	void CreateFromString(const char* vertexCode, const char* fragmentCode);
	void CreateFromFiles(const char* vertexLocation, const char* fragmentLocation);
	void CreateFromFiles(const char* vertexLocation, const char* geometryLocation, const char* fragmentLocation);
	void CreateFromFiles(const char* vertexLocation, const char* tesscontrolLocation, const char* tessevaluationLocation, const char* fragmentLocation);
	void CreateFromFiles(const char* vertexLocation, const char* tesscontrolLocation, const char* tessevaluationLocation,const char* geometryLocation, const char* fragmentLocation);
	void CreateFromFiles(const char* computeLocation);

	virtual void Validate();

	std::string ReadFile(const char* fileLocation);

	GLuint GetProjectionLocation();
	GLuint GetModelLocation();
	GLuint GetViewLocation();
	GLuint GetPrevPVMLocation();
	GLuint GetAmbientColorLocation();
	GLuint GetDirectionLocation();
	GLuint GetEyePositionLocation();
	GLuint GetHeightScaleLocation();
	GLuint GetAlbedoLocation();
	GLuint GetMetallicLocation();
	GLuint GetNormalLocation();
	GLuint GetRoughnessLocation();
	GLuint GetParallaxLocation();
	GLuint GetGlowLocation();
	GLuint GetOmniLightPosLocation();
	GLuint GetFarPlaneLocation();
	GLuint GetShaderID();

	virtual void SetDirectionalLight(DirectionalLight* dLight);
	virtual void SetPointLight(PointLight* pLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset);
	virtual void SetSpotLight(SpotLight* sLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset);
	virtual void SetTexture(GLuint textureUnit);
	virtual void SetSkybox(GLuint textureUnit);
	virtual void SetIrradianceMap(GLuint textureUnit);
	virtual void SetPrefilterMap(GLuint textureUnit);
	virtual void SetBRDFLUT(GLuint textureUnit);
	virtual void SetDirectionalShadowMap(GLuint textureUnit);
	virtual void SetAOMap(GLuint textureUnit);
	virtual void SetDirectionalLightTransform(glm::mat4* lTransform);
	virtual void SetLightMatrices(std::vector<glm::mat4> lightMatrices);

	void UseShader();
	void ClearShader();

	virtual ~Shader() = 0;

protected:
	int pointLightCount = 0;
	int spotLightCount = 0;

	GLuint shaderID = 0, theShader = 0, uniformProjection = -1, uniformModel = -1, uniformView = -1, uniformPrevPVM = -1, uniformEyePosition = -1, uniformHeightScale = -1,
		uniformSkybox = -1, uniformIrradiance = -1, uniformPrefilter = -1, uniformBRDF = -1,
		uniformAlbedo = -1, uniformMetallic = -1, uniformNormal = -1, uniformRoughness = -1, uniformParallax = -1, uniformGlow = -1, uniformAO = -1,
		uniformTexture = -1,
		uniformDirectionalLightTransform = -1, uniformDirectionalShadowMap = -1,
		uniformOmniLightPos = -1, uniformFarPlane = -1;


	GLuint uniformLightMatrices[6]{0};
	GLuint uniformBones[MAX_BONES]{0};

	struct {
		GLuint uniformColor;

		GLuint uniformDirection;
	} uniformDirectionalLight;

	GLuint uniformPointLightCount;

	struct {
		GLuint uniformColor;

		GLuint uniformPosition;
	} uniformPointLight[MAX_POINT_LIGHTS_WITH_SHADOW];

	GLuint uniformSpotLightCount;

	struct {
		GLuint uniformColor;

		GLuint uniformPosition;

		GLuint uniformDirection;
		GLuint uniformEdge;
	}uniformSpotLight[MAX_SPOT_LIGHTS];

	struct {
		GLuint shadowMap;
		GLuint farPlane;

	}uniformOmniShadowMap[MAX_POINT_LIGHTS_WITH_SHADOW + MAX_SPOT_LIGHTS];

	void CompileShader(const char* vertexCode, const char* fragmentCode);
	void CompileShader(const char* vertexCode, const char* geometryCode, const char* fragmentCode);
	void CompileShader(const char* vertexCode, const char* tessellationControlCode, const char* tessellationEvaluationCode, const char* fragmentCode);
	void CompileShader(const char* vertexCode, const char* tessellationControlCode, const char* tessellationEvaluationCode,const char* geometryCode, const char* fragmentCode);
	void CompileShader(const char* computeCode);
	void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);

	virtual void CompileProgram();
};

