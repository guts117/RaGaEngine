#pragma once

#include <stdio.h>
#include <string>
#include <cmath>
#include <memory>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "CommonValues.h"

#include "Window.h"
#include "Equirectangular_to_CubeMap_Shader.h"
#include "Irradiance_Convolution_Shader.h"
#include "PreFilter_Shader.h"
#include "BRDF_Shader.h"
#include "PreZPass_Shader.h"
#include "Terrain_PreZPass_Shader.h"
#include "SSAO_Shader.h"
#include "SSAOBlur_Shader.h"
#include "Model_Shader.h"
#include "Terrain_Shader.h"
#include "Billboard_Shader.h"
#include "Particle_Shader.h"
#include "HDR_Shader.h"
#include "Blur_Shader.h"
#include "MotionBlur_Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

#include "Equirectangular_to_CubeMap_Framebuffer.h"
#include "PreFilter_Framebuffer.h"
#include "BRDF_Framebuffer.h"
#include "Depth_Framebuffer.h"
#include "SSAO_Framebuffer.h"
#include "SSAOBlur_Framebuffer.h"
#include "HDR_Framebuffer.h"
#include "Motion_Blur_FrameBuffer.h"

#include "Blur_PingPong_Framebuffer.h"

#include "Static_Model.h"
#include "Animated_Model.h"

#include "Static_Object.h"

#include "ParticleSystem.h"
#include "Skybox.h"

#include <assimp/Importer.hpp>



class Game {

public:
	Game();

	void init();

	void update(float fps);
	Window mainWindow = Window(ScreenWidth, ScreenHeight);
	~Game();
private:
	void calcAverageNormals(unsigned int* indices, unsigned int indicesCount,
		GLfloat* vertices, unsigned int verticesCount,
		unsigned int vLength, unsigned int normalOffset);

	void calcAverageTangents(unsigned int* indices, unsigned int indicesCount,
		GLfloat* vertices, unsigned int verticesCount,
		unsigned int vLength, unsigned int tangentOffset);

	void CreateBillboard();
	void CreateParticles();
	void CreateTerrain();
	void CreateObject();
	void CreateShaders();
	void RenderBillboardScene();
	void RenderParticlesScene(GLfloat deltaTime);
	void RenderTerrain(bool  shadow, bool depth);
	void RenderEnvCubeMap(bool is_cubeMap);
	void RenderScene(std::shared_ptr<Shader> shader);
	void RenderAnimScene(bool shadow, bool depth);

	void EnvironmentMapPass();
	void IrradianceConvolutionPass();
	void PrefilterPass();
	void BRDFPass();
	void PreZPass(GLfloat deltaTime);
	void SSAOPass();
	void SSAOBlurPass();
	void DirectionalShadowMapPass(DirectionalLight* light);
	void OmniShadowMapPass(PointLight* light);
	void RenderPass(GLfloat deltaTime);
	void MotionBlurPass(float fps);
	void BlurPass();
	void BloomPass();
	const float toRadians = static_cast<float>(M_PI) / 180.0f;

	GLuint uniformProjectionEnv = 0, uniformViewEnv = 0;
	GLuint uniformProjectionIrr = 0, uniformViewIrr = 0;
	GLuint uniformProjectionPreF = 0, uniformViewPreF = 0;
	
	GLuint uniformProjectionAO = 0, uniformSampleRadius = 0;

	GLuint uniformModel = 0, uniformProjection = 0, uniformView = 0, uniformPrevPVM = 0, uniformEyePosition = 0, uniformHeightScale = 0,
	    uniformAlbedoMap = 0, uniformMetallicMap = 0, uniformNormalMap = 0, uniformRoughnessMap = 0, uniformParallaxMap = 0, uniformGlowMap = 0,
		uniformOmniLightPos = 0, uniformFarPlane = 0;

	GLuint uniformModel1 = 0, uniformProjection1 = 0, uniformView1 = 0, uniformPrevPVM1 = 0, uniformEyePosition1 = 0, uniformHeightScale1 = 0,
		uniformAlbedoMap1 = 0, uniformMetallicMap1 = 0, uniformNormalMap1 = 0, uniformRoughnessMap1 = 0, uniformParallaxMap1 = 0, uniformGlowMap1 = 0,
		uniformOmniLightPos1 = 0, uniformFarPlane1 = 0;

	GLuint uniformBones[MAX_BONES] = { 0 };

	GLuint uniformModel2 = 0, uniformProjection2 = 0, uniformView2 = 0, uniformPrevPVM2 = 0, uniformEyePosition2 = 0, uniformHeightScale2 = 0, uniformDispFactor = 0,
		uniformAlbedoMap2 = 0, uniformMetallicMap2 = 0, uniformRoughnessMap2 = 0, uniformNormalMap2 = 0, uniformParallaxMap2 = 0,
		uniformOmniLightPos2 = 0, uniformFarPlane2 = 0;

	GLuint uniformProjection0 = 0, uniformView0 = 0, uniformPrevPV0 = 0, uniformCameraUp = 0, uniformCameraRight = 0, uniformPos = 0, uniformSize = 0;

	GLuint uniformProjectionParticles = 0, uniformViewParticles = 0, uniformCameraUpParticles = 0, uniformCameraRightParticles = 0;

	GLuint uniformHDR = 0, uniformExposure = 0, uniformBlur = 0;

	GLuint uniformHorizontal = 0, uniformWeight = 0;

	GLuint uniformVelocityScale = 0;

	std::unique_ptr<Equirectangular_to_CubeMap_Shader> environmentMapShader = std::make_unique<Equirectangular_to_CubeMap_Shader>();
	std::unique_ptr<Equirectangular_to_CubeMap_Framebuffer> environmentMap;

	std::unique_ptr <Irradiance_Convolution_Shader> irradianceConvolutionShader = std::make_unique<Irradiance_Convolution_Shader>();
	std::unique_ptr <Equirectangular_to_CubeMap_Framebuffer> irradianceMap;

	std::unique_ptr <PreFilter_Shader> prefilterShader = std::make_unique<PreFilter_Shader>();
	std::unique_ptr <PreFilter_Framebuffer> prefilterMap;

	std::unique_ptr < BRDF_Shader > brdfShader = std::make_unique<BRDF_Shader>();
	std::unique_ptr < BRDF_Framebuffer > brdfMap;

	std::shared_ptr < Model_Shader > directionalShadowShader = std::make_shared<Model_Shader>();
	std::shared_ptr < Model_Shader > omniShadowShader = std::make_shared<Model_Shader>();

	std::shared_ptr < Model_Shader > animDirectionalShadowShader = std::make_shared<Model_Shader>();
	std::shared_ptr < Model_Shader > animOmniShadowShader = std::make_shared<Model_Shader>();

	std::shared_ptr < Terrain_Shader> terrainDirectionalShadowShader = std::make_shared<Terrain_Shader>();
	std::shared_ptr < Terrain_Shader> terrainOmniDirectionalShadowShader = std::make_shared<Terrain_Shader>();

	std::shared_ptr < PreZPass_Shader> static_preZPassShader = std::make_shared<PreZPass_Shader>();
	std::shared_ptr < PreZPass_Shader> anim_preZPassShader = std::make_shared<PreZPass_Shader>();
	std::shared_ptr < Terrain_PreZPass_Shader> terrain_preZPassShader = std::make_unique<Terrain_PreZPass_Shader>();
	std::unique_ptr < Depth_Framebuffer> depth = nullptr;

	std::shared_ptr < SSAO_Shader> ssaoShader = std::make_unique<SSAO_Shader>();
	std::unique_ptr < SSAO_Framebuffer> ssao = nullptr;

	std::shared_ptr < SSAOBlur_Shader > ssaoBlurShader = std::make_shared<SSAOBlur_Shader>();
	std::unique_ptr < SSAOBlur_Framebuffer > ssaoBlur = nullptr;

	std::vector< std::shared_ptr < Model_Shader>> shaderList;
	std::vector< std::shared_ptr < Model_Shader>> animShaderList;

	std::shared_ptr < Terrain_Shader> terrainShader = std::make_shared<Terrain_Shader>();

	std::shared_ptr < Billboard_Shader> billboardShader = std::make_shared<Billboard_Shader>();
	std::shared_ptr < Particle_Shader> particleShader = std::make_shared<Particle_Shader>();

	std::unique_ptr < HDR_Shader> hdrShader = std::make_unique<HDR_Shader>();
	std::unique_ptr < HDR_Framebuffer> hdr = nullptr;
	std::unique_ptr < MotionBlur_Shader> motionBlurShader = std::make_unique<MotionBlur_Shader>();
	std::unique_ptr < Motion_Blur_FrameBuffer> motionBlur = nullptr;
	std::unique_ptr < Blur_Shader> blurShader = std::make_unique<Blur_Shader>();
	std::unique_ptr < Blur_PingPong_Framebuffer> blur = nullptr;

	std::vector< std::shared_ptr < Static_Mesh>> meshList;
	std::vector< std::shared_ptr < Static_Mesh>> terrainList;
	std::vector< std::shared_ptr < Static_Mesh>> billboardList;
	std::vector< std::shared_ptr < ParticleSystem>> particleList;

	std::unique_ptr < Static_Mesh> quad;
	std::unique_ptr < Static_Mesh> mesh_cube;

	std::shared_ptr < Camera> camera;

	std::shared_ptr < DirectionalLight> mainLight;
	std::shared_ptr < PointLight> pointLights[MAX_POINT_LIGHTS];
	std::shared_ptr < SpotLight> spotLights[MAX_SPOT_LIGHTS];

	unsigned int pointLightCount = 0;
	unsigned int spotLightCount = 0;

	std::unique_ptr < Skybox> skybox;

	std::unique_ptr < Texture> environmentTexture;
	std::unique_ptr < Texture> skyboxTexture;

	std::unique_ptr < Texture> brickTexture;
	std::unique_ptr < Texture> brickTextureMetal;
	std::unique_ptr < Texture> brickTextureNorm;
	std::unique_ptr < Texture> brickTextureRough;
	std::unique_ptr < Texture> brickTexturePara;
	std::unique_ptr < Texture> brickTextureGlow;
	std::unique_ptr < Texture> metalDebrisTexture;
	std::unique_ptr < Texture> metalDebrisTextureMetal;
	std::unique_ptr < Texture> metalDebrisTextureNorm;
	std::unique_ptr < Texture> metalDebrisTextureRough;
	std::unique_ptr < Texture> metalDebrisTexturePara;
	std::unique_ptr < Texture> metalDebrisTextureGlow;
	std::unique_ptr < Texture> floorTexture;
	std::unique_ptr < Texture> floorTextureMetal;
	std::unique_ptr < Texture> floorTextureNorm;
	std::unique_ptr < Texture> floorTextureRough;
	std::unique_ptr < Texture> floorTexturePara;
	std::unique_ptr < Texture> floorTextureGlow;
	std::unique_ptr < Texture> rustedMetal;
	std::unique_ptr < Texture> rustedMetalMetal;
	std::unique_ptr < Texture> rustedMetalNorm;
	std::unique_ptr < Texture> rustedMetalRough;
	std::unique_ptr < Texture> rustedMetalPara;
	std::unique_ptr < Texture> rustedMetalGlow;

	std::unique_ptr < Texture> terrainTextureDisp;
	std::unique_ptr < Texture> terrainTextureBlend;
	std::unique_ptr < Texture>	terrainTexture;
	std::unique_ptr < Texture> terrainTextureMetal;
	std::unique_ptr < Texture> terrainTextureRough;
	std::unique_ptr < Texture> terrainTextureNorm;
	std::unique_ptr < Texture> terrainTexturePara;

	std::unique_ptr < Texture> SSAONoiseTexture = std::make_unique<Texture>();

	std::unique_ptr < Texture> plainTexture;
	std::unique_ptr < Texture> grassTexture;

	std::shared_ptr < Material> shinyMaterialGlow;
	std::unique_ptr < Material> dullMaterialGlow;
	std::unique_ptr < Material> shinyMaterialPara;
	std::unique_ptr < Material> dullMaterialPara;
	std::unique_ptr < Material> shinyMaterialRough;
	std::unique_ptr < Material> dullMaterialRough;
	std::unique_ptr < Material> shinyMaterialNorm;
	std::unique_ptr < Material> dullMaterialNorm;
	std::unique_ptr < Material> shinyMaterialMetal;
	std::unique_ptr < Material> dullMaterialMetal;
	std::unique_ptr < Material> shinyMaterial;
	std::unique_ptr < Material> dullMaterial;

	std::unique_ptr < Material> shinyTerrainMaterial;
	std::unique_ptr < Material> dullTerrainMaterial;

	std::unique_ptr < Animated_Model> anim = std::make_unique<Animated_Model>();
	std::unique_ptr < Animated_Model> anim2 = std::make_unique<Animated_Model>();
	
	std::unique_ptr <Static_Object> pyramid1 = std::make_unique<Static_Object>();
	std::unique_ptr <Static_Object> pyramid2 = std::make_unique<Static_Object>();
	std::unique_ptr <Static_Object> rectangle1 = std::make_unique<Static_Object>();
	std::unique_ptr <Static_Object> rectangle2 = std::make_unique<Static_Object>();

	std::unique_ptr <Static_Object> sniper = std::make_unique<Static_Object>();
	std::unique_ptr <Static_Object> gun = std::make_unique<Static_Object>();
	std::unique_ptr <Static_Object> anymodel = std::make_unique<Static_Object>();
	std::unique_ptr <Static_Object> cube = std::make_unique<Static_Object>();
	std::unique_ptr <Static_Object> sphere = std::make_unique<Static_Object>();
	std::unique_ptr <Static_Object> bulb = std::make_unique<Static_Object>();
	
	GLfloat deltaTime = 0.0f;
	GLfloat lastTime = 0.0f;

	GLfloat aircraftAngle = 0.0f;

	//Vertex Shader
	const std::string vShader = "Shaders/shader.vert";

	//Fragment Shaders
	const std::string fShader = "Shaders/shader.frag";

	const std::string avShader = "Shaders/animated_shader.vert";

	std::vector<glm::vec3> ssaoNoiseData{16, glm::vec3(0.0f, 0.0f, 0.0f) };

	glm::mat4 vView[NUM_CASCADES] = { glm::mat4() };
	glm::mat4 testLitView[1] = { glm::mat4() };

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[6] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
	};
};