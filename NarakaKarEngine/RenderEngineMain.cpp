#include "pch.h"
#include "RenderEngineMain.h"

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
#include "Compute_Shader.h"

#include "Mesh.h"
#include "Static_Mesh.h"
#include "Billboard_Mesh.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "Particle.h"

#include "Scene_Fbo_Handler_Manager.h"
#include "Fbo_Handler.h"

#include "Static_Model.h"
#include "Animated_Model.h"

#include "Static_Object.h"

#include "ParticleSystem.h"
#include "Skybox.h"

#include "Debug.h"

#include "RenderingCommonValues.h"
#include "MathUtil.h"
#include "Window.h"

#include "PhysicsEngineMain.h"
#include "EngineUIMain.h"
#include "Shader_Object.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;
using namespace EngineUI;

extern int ScreenWidth;
extern int ScreenHeight;
extern bool isUpdateFrameBuffersSize;
extern std::unique_ptr<PhysicsEngineMain> physicsEngine;
extern std::unique_ptr<EngineUIMain> engineUI;

struct RenderEngineMain::Impl
{
	Impl() = default;

	Impl(Impl&& rhs) = delete;
	Impl& operator=(Impl&& rhs) = delete;

	Impl(const Impl& rhs) = delete;
	Impl& operator=(const Impl& rhs) = delete;


	std::unique_ptr<Window> mainWindow = std::make_unique<Window>();

	bool direction = true;
	float triOffset = 0.0f;
	float triMaxOffset = 0.6f;
	float triIncrement = 0.005f;

	float curAngle = 0.0f;

	bool tooSmall = true;
	float curScale = 0.0f;
	float scaleIncrement = 0.0f;
	float scaleMax = 5.0f;
	float scaleMin = -1.0f;
	bool drawFluidSim = false;
	bool drawSmokeSim = false;

	float terrainScaleFactor = 0.0f;
	float terrainScaleFactor1 = 1000.0f;

	const float toRadians = static_cast<float>(M_PI) / 180.0f;

	GLuint uniformModel1 = 0, uniformProjection1 = 0, uniformView1 = 0, uniformPrevPVM1 = 0, uniformEyePosition1 = 0, uniformHeightScale1 = 0,
		uniformAlbedoMap1 = 0, uniformMetallicMap1 = 0, uniformNormalMap1 = 0, uniformRoughnessMap1 = 0, uniformParallaxMap1 = 0, uniformGlowMap1 = 0,
		uniformOmniLightPos1 = 0, uniformFarPlane1 = 0;

	GLuint uniformBones[MAX_BONES] = { 0 };

	GLuint uniformModel2 = 0, uniformProjection2 = 0, uniformView2 = 0, uniformPrevPVM2 = 0, uniformEyePosition2 = 0, uniformHeightScale2 = 0, uniformDispFactor = 0,
		uniformAlbedoMap2 = 0, uniformMetallicMap2 = 0, uniformRoughnessMap2 = 0, uniformNormalMap2 = 0, uniformParallaxMap2 = 0,
		uniformOmniLightPos2 = 0, uniformFarPlane2 = 0;

	std::unique_ptr<Compute_Shader> buildAABBGridCompShader = std::make_unique<Compute_Shader>();
	std::unique_ptr<Compute_Shader> visibleClusterCompShader = std::make_unique<Compute_Shader>();
	std::unique_ptr<Compute_Shader> cullLightsCompShader = std::make_unique <Compute_Shader>();

	//ToDo: #20 simulation manager class
	/*std::unique_ptr<Compute_Shader> fluidFinalShader = std::make_unique <Compute_Shader>();
	std::unique_ptr<Compute_Shader> addSmokeSpotCompShader = std::make_unique <Compute_Shader>();
	std::unique_ptr<Compute_Shader> maxReduceCompShader = std::make_unique <Compute_Shader>();
	std::unique_ptr<Compute_Shader> RKCompShader = std::make_unique <Compute_Shader>();
	std::unique_ptr<Compute_Shader> maccormackCompShader = std::make_unique <Compute_Shader>();
	std::unique_ptr<Compute_Shader> jacobiBlackCompShader = std::make_unique <Compute_Shader>();
	std::unique_ptr<Compute_Shader> jacobiRedCompShader = std::make_unique <Compute_Shader>();
	std::unique_ptr<Compute_Shader> divRBCompShader = std::make_unique <Compute_Shader>();
	std::unique_ptr<Compute_Shader> pressureProjectionRBCompShader = std::make_unique <Compute_Shader>();
	std::unique_ptr<Compute_Shader> buoyantForceCompShader = std::make_unique<Compute_Shader>();
	std::unique_ptr <Billboard_Shader> fluidFragShader = std::make_unique<Billboard_Shader>();

	std::unique_ptr <Compute_Shader> clearTexCompShader3D = std::make_unique<Compute_Shader>();
	std::unique_ptr <Compute_Shader> addSmokeSpotCompShader3D = std::make_unique<Compute_Shader>();
	std::unique_ptr <Compute_Shader> RKAdvectCompShader3D = std::make_unique<Compute_Shader>();
	std::unique_ptr <Compute_Shader> maccormackCompShader3D = std::make_unique<Compute_Shader>();
	std::unique_ptr<Compute_Shader> buoyantForceCompShader3D = std::make_unique<Compute_Shader>();
	std::unique_ptr<Compute_Shader> divCompShader3D = std::make_unique <Compute_Shader>();
	std::unique_ptr<Compute_Shader> jacobiCompShader3D = std::make_unique <Compute_Shader>();
	std::unique_ptr<Compute_Shader> pressureProjectionCompShader3D = std::make_unique <Compute_Shader>();
	std::unique_ptr <Model_Shader> fluidFragShader3D = std::make_unique<Model_Shader>();*/

	std::shared_ptr<Scene_Fbo_Handler_Manager> m_SceneFboHandlerMgr;

	std::unique_ptr<Equirectangular_to_CubeMap_Shader> environmentMapShader = std::make_unique<Equirectangular_to_CubeMap_Shader>();
	std::unique_ptr <Irradiance_Convolution_Shader> irradianceConvolutionShader = std::make_unique<Irradiance_Convolution_Shader>();
	std::unique_ptr <PreFilter_Shader> prefilterShader = std::make_unique<PreFilter_Shader>();
	std::unique_ptr < BRDF_Shader > brdfShader = std::make_unique<BRDF_Shader>();

	std::shared_ptr <Fbo_Handler> environmentMap;
	std::shared_ptr <Fbo_Handler> irradianceMap;
	std::shared_ptr <Fbo_Handler> prefilterMap;
	std::shared_ptr <Fbo_Handler> brdfMap;
	std::shared_ptr <Fbo_Handler> depth;
	std::shared_ptr <Fbo_Handler> ssao;
	std::shared_ptr <Fbo_Handler> ssaoBlur;
	std::shared_ptr <Fbo_Handler> hdr;
	std::shared_ptr <Fbo_Handler> motionBlur;
	std::shared_ptr <Fbo_Handler> blur;
	std::shared_ptr <Fbo_Handler> finalFBO;

	std::shared_ptr < Model_Shader > directionalShadowShader = std::make_shared<Model_Shader>();
	std::shared_ptr < Model_Shader > omniShadowShader = std::make_shared<Model_Shader>();

	std::shared_ptr < Model_Shader > animDirectionalShadowShader = std::make_shared<Model_Shader>();
	std::shared_ptr < Model_Shader > animOmniShadowShader = std::make_shared<Model_Shader>();

	std::shared_ptr < Terrain_Shader> terrainDirectionalShadowShader = std::make_shared<Terrain_Shader>();
	std::shared_ptr < Terrain_Shader> terrainOmniDirectionalShadowShader = std::make_shared<Terrain_Shader>();

	std::shared_ptr < PreZPass_Shader> static_preZPassShader = std::make_shared<PreZPass_Shader>();
	std::unique_ptr < PreZPass_Shader> anim_preZPassShader = std::make_unique<PreZPass_Shader>();
	std::unique_ptr < Terrain_PreZPass_Shader> terrain_preZPassShader = std::make_unique<Terrain_PreZPass_Shader>();

	std::unique_ptr < SSAO_Shader> ssaoShader = std::make_unique<SSAO_Shader>();

	std::unique_ptr < SSAOBlur_Shader > ssaoBlurShader = std::make_unique<SSAOBlur_Shader>();

	std::vector< std::shared_ptr < Model_Shader>> shaderList;
	std::vector< std::shared_ptr < Model_Shader>> animShaderList;

	std::unique_ptr < Terrain_Shader> terrainShader = std::make_unique<Terrain_Shader>();

	std::unique_ptr < Shader_Object > billboardShader;
	std::unique_ptr < Particle_Shader> particleShader = std::make_unique<Particle_Shader>();

	std::unique_ptr < HDR_Shader> hdrShader = std::make_unique<HDR_Shader>();
	std::unique_ptr < MotionBlur_Shader> motionBlurShader = std::make_unique<MotionBlur_Shader>();
	std::unique_ptr < Blur_Shader> blurShader = std::make_unique<Blur_Shader>();

	std::vector< std::shared_ptr < Static_Mesh>> meshList;
	std::vector< std::shared_ptr < Static_Mesh>> terrainList;
	std::vector< std::shared_ptr < Billboard_Mesh>> billboardList;
	std::vector< std::shared_ptr < ParticleSystem>> particleList;

	std::unique_ptr < Static_Mesh> quad;
	std::unique_ptr < Static_Mesh> mesh_cube;
	std::unique_ptr < Static_Mesh> ccw_cube;

	std::shared_ptr < Camera> camera;

	std::shared_ptr < DirectionalLight> mainLight;
	std::shared_ptr < PointLight> pointLights[MAX_POINT_LIGHTS_WITH_SHADOW];
	std::shared_ptr < SpotLight> spotLights[MAX_SPOT_LIGHTS];

	unsigned int pointLightCount = 0;
	unsigned int spotLightCount = 0;

	std::unique_ptr < Skybox> skybox;

	std::unique_ptr < Texture> environmentTexture;
	std::unique_ptr < Texture> skyboxTexture;

	std::unique_ptr < Texture> terrainTextureDisp;
	std::unique_ptr < Texture> terrainTextureBlend;
	std::unique_ptr < Texture> terrainTexture;
	std::unique_ptr < Texture> terrainTextureMetal;
	std::unique_ptr < Texture> terrainTextureRough;
	std::unique_ptr < Texture> terrainTextureNorm;
	std::unique_ptr < Texture> terrainTexturePara;
	
	//ToDo: #20 simulation manager class
	//std::unique_ptr<Texture> velocitiesTexture;
	//std::unique_ptr<Texture> density;
	//std::unique_ptr<Texture> divRBTexture;
	//std::unique_ptr<Texture> pressureRBTexture;
	//std::unique_ptr<Texture> finalReduceTexture;

	//std::unique_ptr<Texture> smokeVelocitiesTexture;
	//std::unique_ptr<Texture> smokeDensity;
	//std::unique_ptr<Texture> smokeDivRBTexture;
	//std::unique_ptr<Texture> smokePressureRBTexture;
	//std::unique_ptr<Texture> smokeTemperatureTexture;
	//std::unique_ptr<Texture> smokeFinalReduceTexture;

	//std::unique_ptr<std::vector<std::unique_ptr<Texture>>> density3D;
	//std::unique_ptr<std::vector<std::unique_ptr<Texture>>> veloxity3D;
	//std::unique_ptr<std::vector<std::unique_ptr<Texture>>> temperature3D;
	//std::unique_ptr<Texture> div3D;
	//std::unique_ptr<Texture> pressure3D;
 
	std::unique_ptr < Texture> SSAONoiseTexture = std::make_unique<Texture>();

	std::unique_ptr < Texture> plainTexture;
	std::unique_ptr < Texture> grassTexture;

	std::unique_ptr < Material> shinyMaterialGlow;
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
	std::unique_ptr <Static_Object> bulbWhite = std::make_unique<Static_Object>();
	std::unique_ptr <Static_Object> bulbRed = std::make_unique<Static_Object>();

	GLfloat aircraftAngle = 0.0f;

	const std::string vShader = "Shaders/shader.vert";
	const std::string fShader = "Shaders/shader.frag";
	const std::string avShader = "Shaders/animated_shader.vert";

	std::vector<glm::vec3> ssaoNoiseData{ 16, glm::vec3(0.0f, 0.0f, 0.0f) };

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

	unsigned int AABBvolumeGridSSBO, lightSSBO, lightIndexListSSBO, lightGridSSBO, visibleClusterSSBO, screenToViewSSBO;
	
	int gridSizeX = 32;
	int gridSizeY = 32;
	int gridSizeZ = 32;
	unsigned int sizeX, sizeY;
	int numClusters = gridSizeX * gridSizeY * gridSizeZ;

	unsigned int numLights;
	const unsigned int maxLights = 1000;
	const unsigned int maxLightsPerTile = 50;

	struct VolumeTileAABB {
		glm::vec4 minPoint;
		glm::vec4 maxPoint;
	};

	struct ScreenToView {
		glm::mat4 inverseProjectionMat;
		unsigned int tileSizes[4];
		unsigned int tileSizeInPixel[2];
		unsigned int screenWidth;
		unsigned int screenHeight;
		float sliceScalingFactor;
		float sliceBiasFactor;
		float zNear;
		float zFar;
	};

	struct GPULight {
		glm::vec4 position = glm::vec4();
		glm::vec4 color = glm::vec4();
		unsigned int enabled = 0;
		float intensity = 0;
		float range = 0;
		float padding = 0;
	};

	GLfloat lastTime = 0.0f;
	GLfloat framesPerSec = 0.0f;
	GLfloat lastFrameTime = 0.0f;
	GLfloat deltaTime = 0.0f;

	std::unique_ptr<std::vector<std::function<void(int, int)>>> resizeUpdateFramebuffers;

	int velTexId[4] = { 0 , 1, 2, 3 };
	int denTexId[4] = { 0 , 1, 2, 3 };
	int smokeVelTexId[4] = { 0 , 1, 2, 3 };
	int smokeDenTexId[4] = { 0 , 1, 2, 3 };
	int smokeTempTexId[4] = { 0, 1, 2, 3 };
	double sOriginX, sOriginY;
	int simWidth = 1024;
	int simHeight = 1024;
	bool addSplat = false;
	float simDt = 0.0f;
	float simDt3D = 0.0f;
	int simDim3D = 128;

	bool isGameViewSelected;
	bool isEditorViewSelected;


	void Init()
	{
		glEnable(GL_TEXTURE_3D);
		mainWindow->Initialise();
		lastFrameTime = static_cast<GLfloat>(glfwGetTime());
		CreateBillboard();
		CreateParticles();
		CreateTerrain();
		CreateObject();
		CreateShaders();

		camera = std::make_shared<Camera>(glm::vec3(-terrainScaleFactor, 40.0f, -terrainScaleFactor + 40.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 50.0f, 0.2f);

		environmentTexture = std::make_unique<Texture>("Textures/HDR/GCanyon_C_YumaPoint_3k.hdr");
		environmentTexture->LoadTextureHDR();

		plainTexture = std::make_unique <Texture>("Textures/plain.png", true);
		plainTexture->LoadTextureWithAlpha();

		grassTexture = std::make_unique <Texture>("Textures/grass.png", true);
		grassTexture->LoadTextureWithAlpha();


		pyramid1 = std::make_unique<Static_Object>();
		pyramid1->SetUpNativeModelData(meshList[0], "Textures/rustediron2.png", "Textures/Metallic/rustediron2.png",
			"Textures/Roughness/rustediron2.png", "Textures/Normal/rustediron2.png",
			"Textures/Parallax/rustediron2.png", "Textures/Glow/rock.jpg");
		pyramid2 = std::make_unique<Static_Object>();
		pyramid2->SetUpNativeModelData(meshList[1], "Textures/small_metal_debris.jpg", "Textures/Metallic/small_metal_debris.jpg",
			"Textures/Roughness/small_metal_debris.jpg", "Textures/Normal/small_metal_debris.jpg",
			"Textures/Parallax/small_metal_debris.jpg", "Textures/Glow/small_metal_debris.jpg");
		rectangle1 = std::make_unique<Static_Object>();
		rectangle1->SetUpNativeModelData(meshList[2], "Textures/brick.jpg", "Textures/Metallic/brick.jpg",
			"Textures/Roughness/brick.jpg", "Textures/Normal/brick.jpg",
			"Textures/Parallax/brick.jpg", "Textures/Glow/brick.jpg");
		rectangle2 = std::make_unique<Static_Object>();
		rectangle2->SetUpNativeModelData(meshList[3], "Textures/brick_floor.png", "Textures/Metallic/brick_floor.png",
			"Textures/Roughness/brick_floor.png", "Textures/Normal/brick_floor.png",
			"Textures/Parallax/brick_floor.png", "Textures/Glow/brick_floor.png");


		terrainTextureDisp = std::make_unique <Texture>("Textures/Displacement/terrain.jpg");
		terrainTextureDisp->LoadTextureNoAlpha();
		terrainTextureBlend = std::make_unique <Texture>("Textures/Blend/terrain.jpg");
		terrainTextureBlend->LoadTextureNoAlpha();
		terrainTexture = std::make_unique <Texture>("Textures/terrain.jpg", true);
		terrainTexture->LoadTextureArray(glm::vec2(1024, 1024), NUM_TERRAIN_LAYERS);
		terrainTextureMetal = std::make_unique <Texture>("Textures/Metallic/terrain.jpg");
		terrainTextureMetal->LoadTextureArray(glm::vec2(256, 256), NUM_TERRAIN_LAYERS);
		terrainTextureRough = std::make_unique <Texture>("Textures/Roughness/terrain.jpg");
		terrainTextureRough->LoadTextureArray(glm::vec2(256, 256), NUM_TERRAIN_LAYERS);
		terrainTextureNorm = std::make_unique <Texture>("Textures/Normal/terrain.jpg");
		terrainTextureNorm->LoadTextureArray(glm::vec2(256, 256), NUM_TERRAIN_LAYERS);
		terrainTexturePara = std::make_unique <Texture>("Textures/Parallax/terrain.jpg");
		terrainTexturePara->LoadTextureArray(glm::vec2(256, 256), NUM_TERRAIN_LAYERS);

		//ToDo: #20 simulation manager class
		/*velocitiesTexture = std::make_unique <Texture>();
		velocitiesTexture->CreateTextureArray(glm::uvec2(simWidth, simHeight) , 4);
		density = std::make_unique <Texture>();
		density->CreateTextureArray(glm::uvec2(simWidth, simHeight), 4);
		divRBTexture = std::make_unique <Texture>();
		divRBTexture->CreateTexture(glm::uvec2(simWidth / 2, simHeight / 2));
		pressureRBTexture = std::make_unique <Texture>();
		pressureRBTexture->CreateTexture(glm::uvec2(simWidth / 2, simHeight / 2));
		finalReduceTexture = std::make_unique<Texture>();
		finalReduceTexture->CreateTexture(glm::uvec2(simWidth / 32, simWidth / 32));

		smokeVelocitiesTexture = std::make_unique<Texture>();
		smokeVelocitiesTexture->CreateTextureArray(glm::uvec2(simWidth, simHeight), 4);
		smokeDensity = std::make_unique<Texture>();
		smokeDensity->CreateTextureArray(glm::uvec2(simWidth, simHeight), 4);
		smokeTemperatureTexture = std::make_unique<Texture>();
		smokeTemperatureTexture->CreateTextureArray(glm::uvec2(simWidth, simHeight), 4);
		smokeDivRBTexture = std::make_unique<Texture>();
		smokeDivRBTexture->CreateTexture(glm::uvec2(simWidth / 2, simHeight / 2));
		smokePressureRBTexture = std::make_unique<Texture>();
		smokePressureRBTexture->CreateTexture(glm::uvec2(simWidth / 2, simHeight / 2));
		smokeFinalReduceTexture = std::make_unique<Texture>();
		smokeFinalReduceTexture->CreateTexture(glm::uvec2(simWidth / 32, simWidth / 32));

		density3D = std::make_unique<std::vector<std::unique_ptr<Texture>>>();
		veloxity3D = std::make_unique<std::vector<std::unique_ptr<Texture>>>();
		temperature3D = std::make_unique<std::vector<std::unique_ptr<Texture>>>();*/

		/*for (int i = 0; i < 4; i++)
		{
			density3D.get()->push_back(std::make_unique<Texture>());
			density3D.get()->at(i)->CreateTexture3D(glm::vec3(simDim3D, simDim3D, simDim3D));

			veloxity3D.get()->push_back(std::make_unique<Texture>());
			veloxity3D.get()->at(i)->CreateTexture3D(glm::vec3(simDim3D, simDim3D, simDim3D));

			temperature3D.get()->push_back(std::make_unique<Texture>());
			temperature3D.get()->at(i)->CreateTexture3D(glm::vec3(simDim3D, simDim3D, simDim3D));
		}

		div3D = std::make_unique<Texture>();
		div3D->CreateTexture3D(glm::vec3(simDim3D, simDim3D, simDim3D));

		pressure3D = std::make_unique<Texture>();
		pressure3D->CreateTexture3D(glm::vec3(simDim3D, simDim3D, simDim3D));*/

		shinyMaterialGlow = std::make_unique<Material>(1, 6, 7, 11, 12, 13);
		dullMaterialGlow = std::make_unique<Material>(1, 6, 7, 11, 12, 13);

		shinyMaterialPara = std::make_unique<Material>(1, 6, 7, 11, 12);
		dullMaterialPara = std::make_unique<Material>(1, 6, 7, 11, 12);

		shinyMaterialRough = std::make_unique<Material>(1, 6, 7, 11);
		dullMaterialRough = std::make_unique<Material>(1, 6, 7, 11);

		shinyMaterialNorm = std::make_unique<Material>(1, 6, 7);
		dullMaterialNorm = std::make_unique<Material>(1, 6, 7);

		shinyMaterialMetal = std::make_unique<Material>(1, 6);
		dullMaterialMetal = std::make_unique<Material>(1, 6);

		shinyMaterial = std::make_unique<Material>(1, 1);
		dullMaterial = std::make_unique<Material>(1, 1);

		shinyTerrainMaterial = std::make_unique<Material>(12, 13, 15, 16, 17);
		dullTerrainMaterial = std::make_unique<Material>(12, 13, 15, 16, 17);

		bulbWhite = std::make_unique<Static_Object>();
		bulbWhite->SetUpImportedModelData("Models/Free_Antique_Bulb.obj");
		bulbRed = std::make_unique<Static_Object>();
		bulbRed->SetUpImportedModelData("Models/Free_Antique_Bulb.obj");
		sphere = std::make_unique<Static_Object>();
		sphere->SetUpImportedModelData("Models/sphere.obj");
		physicsEngine->AddSphere(1.0f, -terrainScaleFactor, 80.0f, 5.5f - terrainScaleFactor, 1.0f, sphere->GetModelMatrixForPhysics());
		//ToDo: use heightmap to add terrain collider
		physicsEngine->AddStaticPlane(0.0f, 27.0f, 0.0f, 0.0f, glm::vec3(0.0f, 0.9f, 0.1f), nullptr);
		cube = std::make_unique<Static_Object>();
		cube->SetUpImportedModelData("Models/cube.obj");
		sniper = std::make_unique<Static_Object>();
		sniper->SetUpImportedModelData("Models/Sniper_rifle_KSR-29.fbx");
		gun = std::make_unique<Static_Object>();
		gun->SetUpImportedModelData("Models/Cerberus_LP.fbx");
		anymodel = std::make_unique<Static_Object>();
		anymodel->SetUpImportedModelData("Models/Intergalactic_Spaceship-(Wavefront).obj");
		//anymodel->SetUpImportedModelData("Models/Sponza.gltf");

		anim->LoadModel("Models/boblampclean.md5mesh");
		anim2->LoadModel("Models/model.dae");

		m_SceneFboHandlerMgr = std::make_shared<Scene_Fbo_Handler_Manager>("InGame");

		environmentMap = m_SceneFboHandlerMgr->FindFboHandler("Environment_Map_Pass");
		irradianceMap = m_SceneFboHandlerMgr->FindFboHandler("Irradiance_Map_Pass");
		prefilterMap = m_SceneFboHandlerMgr->FindFboHandler("Pre_Filter_Pass");
		brdfMap = m_SceneFboHandlerMgr->FindFboHandler("Brdf_Pass");
		depth	= m_SceneFboHandlerMgr->FindFboHandler("Depth_Pass");
		ssao	= m_SceneFboHandlerMgr->FindFboHandler("Ssao_Pass");
		ssaoBlur = m_SceneFboHandlerMgr->FindFboHandler("Ssao_Blur_Pass");
		hdr = m_SceneFboHandlerMgr->FindFboHandler("Shading_Pass");
		motionBlur = m_SceneFboHandlerMgr->FindFboHandler("Motion_Blur_Pass");
		blur = m_SceneFboHandlerMgr->FindFboHandler("Bloom_Pass");
		finalFBO = m_SceneFboHandlerMgr->FindFboHandler("Final_Output_Pass");

		quad = std::make_unique < Static_Mesh>();
		mesh_cube = std::make_unique < Static_Mesh>();
		ccw_cube = std::make_unique <Static_Mesh>();

		mainLight = std::make_unique < DirectionalLight>(1024, 1024,
			0.5f, 0.5f, 0.5f,
			5500.0f, -5500.0f, -10000.0f, 
			m_SceneFboHandlerMgr);

		pointLights[0] = std::make_unique < PointLight>(512, 512,
			0.1f, 100.0f,
			0.0f, 0.0f, 3.0f,
			12.0f - terrainScaleFactor, 40.0f, 10.0f - terrainScaleFactor, 
			m_SceneFboHandlerMgr);

		pointLightCount++;

		pointLights[1] = std::make_unique < PointLight>(512, 512,
			0.1f, 100.0f,
			3.0f, 0.0f, 0.0f,
			-12.0f - terrainScaleFactor, 40.0f, 10.0f - terrainScaleFactor, 
			m_SceneFboHandlerMgr);

		pointLightCount++;

		spotLights[0] = std::make_unique < SpotLight>(512, 512,
			0.1f, 100.0f,
			10.0f, 10.0f, 10.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			10.0f, m_SceneFboHandlerMgr);

		spotLightCount++;

		//std::vector<std::string> skyboxFaces;
		//skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
		//skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
		//skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
		//skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
		//skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
		//skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");
		//skyboxFaces.push_back("Textures/Skybox/barren_rt.jpg");
		//skyboxFaces.push_back("Textures/Skybox/barren_lf.jpg");
		//skyboxFaces.push_back("Textures/Skybox/barren_up.jpg");
		//skyboxFaces.push_back("Textures/Skybox/barren_dn.jpg");
		//skyboxFaces.push_back("Textures/Skybox/barren_bk.jpg");
		//skyboxFaces.push_back("Textures/Skybox/barren_ft.jpg");

		//ToDo: #20 simulation manager class
		//clearTexCompShader3D->UseShader();
		//density3D.get()->at(0)->UseTextureReadWrite(0, true, true);
		//clearTexCompShader3D->Dispatch(simDim3D / 32, simDim3D / 32, simDim3D);
		//addSplatSpot3D(glm::vec3(simDim3D / 2, simDim3D / 2, simDim3D / 2), glm::vec3(75.0 / 255.0, 89.0 / 255.0, 1.0), 2.5f, 10.0f, density3D.get()->at(0).get());
		
		/*unsigned x = 300u, y = 400u;
		addSplatSpot(glm::vec2(x, y), glm::vec3(80.0f, 7.0f, 0.0f), 1.0f, velocitiesTexture.get(), velTexId[0]);
		addSplatSpot(glm::vec2(x, y), glm::vec3(75.0 / 255.0, 89.0 / 255.0, 1.0), 2.5f, density.get(), denTexId[0]);

		x = 700u; y = 400u;
		addSplatSpot(glm::vec2(x, y), glm::vec3(-80.0f, -7.0f, 0.0f), 1.0f, velocitiesTexture.get(), velTexId[0]);
		addSplatSpot(glm::vec2(x, y), glm::vec3(1.0, 151.0 / 255.0, 60.0 / 255.0), 2.5f, density.get(), denTexId[0]);

		x = 700u; y = 600u;
		addSplatSpot(glm::vec2(x, y), glm::vec3(-80.0f, -7.0f, 0.0f), 1.0f, velocitiesTexture.get(), velTexId[0]);
		addSplatSpot(glm::vec2(x, y), glm::vec3(151.0 / 255.0, 1.0, 60.0 / 255.0), 2.5f, density.get(), denTexId[0]);

		x = 300u; y = 600u;
		addSplatSpot(glm::vec2(x, y), glm::vec3(80.0f, 7.0f, 0.0f), 1.0f, velocitiesTexture.get(), velTexId[0]);
		addSplatSpot(glm::vec2(x, y), glm::vec3(50.0 / 255.0, 50.0 / 255.0, 50.0 / 255.0), 2.5f, density.get(), denTexId[0]);*/

		skybox = std::make_unique<Skybox>();

		//skyboxTexture.LoadCubeMapSRGB(skyboxFaces);

		InitSSAO();
		CalcDirLightShadowCascades();
		InitSSBOs();
		CreateClusters();
		EnvironmentMapPass();
		IrradianceConvolutionPass();
		PrefilterPass();
		BRDFPass();
	}

	void InitSSAO() 
	{
		//SSAO initialization
		ssaoShader->UseShader();
		ssaoShader->GenKernel();
		ssaoShader->GenNoise(ssaoNoiseData);
		SSAONoiseTexture->LoadNativeTexture(ssaoNoiseData);
	}

	void CalcDirLightShadowCascades() 
	{
		terrainShader->UseShader();
		for (size_t i = 0; i < NUM_CASCADES; ++i)
		{
			glm::vec4 vView(0.0f, 0.0f, mainLight->GetCascadeEnd(i + 1), 1.0f);
			glm::vec4 vClip = camera->GetProjectionMatrix() * vView;
			printf("%F \n", vClip.z);
			terrainShader->SetCascadeEndClipSpace(i, -vClip.z);
		}

		shaderList[0]->UseShader();
		for (size_t i = 0; i < NUM_CASCADES; ++i)
		{
			glm::vec4 vView(0.0f, 0.0f, mainLight->GetCascadeEnd(i + 1), 1.0f);
			glm::vec4 vClip = camera->GetProjectionMatrix() * vView;
			printf("%F \n", vClip.z);
			shaderList[0]->SetCascadeEndClipSpace(i, -vClip.z);
		}

		animShaderList[0]->UseShader();
		for (size_t i = 0; i < NUM_CASCADES; ++i)
		{
			glm::vec4 vView(0.0f, 0.0f, mainLight->GetCascadeEnd(i + 1), 1.0f);
			glm::vec4 vClip = camera->GetProjectionMatrix() * vView;
			printf("%F \n", vClip.z);
			animShaderList[0]->SetCascadeEndClipSpace(i, -vClip.z);
		}
	}
	
	void UpdateAtFrameBufferResize()
	{
		if (isUpdateFrameBuffersSize)
		{
			isUpdateFrameBuffersSize = false;

			auto invProj = glm::inverse(camera->GetProjectionMatrix());
			glNamedBufferSubData(screenToViewSSBO, 0, sizeof(invProj), &invProj);
			sizeX = (unsigned int)std::ceilf(ScreenWidth / (float)gridSizeX);
			sizeY = (unsigned int)std::ceilf(ScreenHeight / (float)gridSizeY);
			int data[4] = { sizeX, sizeY, ScreenWidth, ScreenHeight };
			glNamedBufferSubData(screenToViewSSBO, 80, sizeof(data), &data);

			m_SceneFboHandlerMgr->ResizeScreenFboHandlers(ScreenWidth, ScreenHeight);

			CalcDirLightShadowCascades();
			CreateClusters();
		}
	}

	void Update()
	{
		UpdateAtFrameBufferResize();

		// Measure speed
		GLfloat now = static_cast<GLfloat>(glfwGetTime());
		deltaTime = now - lastTime;
		framesPerSec++;
		lastTime = now;

		if (now - lastFrameTime >= 1.0) {
			// printf and reset timer
			printf("%f  fps, %f  spf \n", framesPerSec, 1000.0 / (double)framesPerSec);
			framesPerSec = 0.0f;
			lastFrameTime += 1.0;
		}

		if (isGameViewSelected) 
		{
			camera->keyControl(mainWindow->getKeys(), deltaTime);
		}

		//ToDo: #20 simulation manager class
		//if (mainWindow->getKeys()[GLFW_KEY_X]) 
		//{
		//	drawFluidSim = !drawFluidSim;
		//	mainWindow->SetCursorActive(drawFluidSim);
		//	if (drawFluidSim) 
		//	{
		//		mainWindow->ResizeWindow(simWidth, simHeight);
		//		drawSmokeSim = false;
		//	}
		//	mainWindow->getKeys()[GLFW_KEY_X] = false;
		//}

		//if (mainWindow->getKeys()[GLFW_KEY_Z])
		//{
		//	drawSmokeSim = !drawSmokeSim;
		//	if (drawSmokeSim)
		//	{
		//		mainWindow->SetCursorActive(false);
		//		mainWindow->ResizeWindow(simWidth, simHeight);
		//		drawFluidSim = false;
		//	}
		//	mainWindow->getKeys()[GLFW_KEY_Z] = false;
		//}
		//if (mainWindow->isLeftMouseRelease) 
		//{
		//	addSplat = false;
		//}

		//if (mainWindow->isLeftMousePress) 
		//{
		//	addSplat = true;
		//}

		if (!drawFluidSim && !drawSmokeSim)
		{
			if (isGameViewSelected)
			{
				camera->mouseControl(mainWindow->getXChange(), mainWindow->getYChange());
			}

			if (direction) {
				triOffset += triIncrement;
			}
			else {
				triOffset -= triIncrement;
			}

			if (abs(triOffset) >= triMaxOffset) {
				direction = !direction;
			}
			curAngle += 0.5f;
			if (curAngle >= 360) {
				curAngle -= 360;
			}

			if (tooSmall) {

				curScale += scaleIncrement;
			}
			else {
				curScale -= scaleIncrement;
			}
			if (curScale >= scaleMax || curScale <= scaleMin) {
				tooSmall = !tooSmall;
			}

			if (mainWindow->getKeys()[GLFW_KEY_L]) {
				spotLights[0]->Toggle();
				mainWindow->getKeys()[GLFW_KEY_L] = false;
			}

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			DirectionalShadowMapPass(mainLight.get());

			for (size_t i = 0; i < pointLightCount; i++) {
				OmniShadowMapPass(pointLights[i].get(), i);
			}
			for (size_t i = 0; i < spotLightCount; i++) {
				OmniShadowMapPass(spotLights[i].get(), pointLightCount + i);
			}

			PreZPass(deltaTime);
			CullLight();
			SSAOPass();
			SSAOBlurPass();
			RenderPass(deltaTime);
			Bloom();
			MotionBlurPass(framesPerSec);
		}

		RenderToDefaultFB();

		camera->UpdatePreviousMatrices();
		glUseProgram(0);
	}

	void EndUpdate() 
	{
		mainWindow->swapBuffers();
	}

	void InitSSBOs() 
	{
		//Setting up tile size on both X and Y 
		sizeX = (unsigned int)std::ceilf(ScreenWidth / (float)gridSizeX);
		sizeY = (unsigned int)std::ceilf(ScreenHeight / (float)gridSizeY);

		//Buffer containing all the clusters
		{
			glGenBuffers(1, &AABBvolumeGridSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, AABBvolumeGridSSBO);

			//We generate the buffer but don't populate it yet.
			glBufferData(GL_SHADER_STORAGE_BUFFER, numClusters * sizeof(struct VolumeTileAABB), NULL, GL_STATIC_COPY);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, AABBvolumeGridSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}

		//Setting up screen2View ssbo
		{
			glGenBuffers(1, &screenToViewSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, screenToViewSSBO);

			ScreenToView screen2View;
			//Setting up contents of buffer
			screen2View.inverseProjectionMat = glm::inverse(camera->GetProjectionMatrix());

			screen2View.tileSizes[0] = gridSizeX;
			screen2View.tileSizes[1] = gridSizeY;
			screen2View.tileSizes[2] = gridSizeZ;
			screen2View.tileSizes[3] = maxLightsPerTile;
			screen2View.tileSizeInPixel[0] = sizeX;
			screen2View.tileSizeInPixel[1] = sizeY;
			screen2View.screenWidth = ScreenWidth;
			screen2View.screenHeight = ScreenHeight;
			//Basically reduced a log function into a simple multiplication an addition by pre-calculating these
			float factor = gridSizeZ / glm::log(camFarZ / camNearZ);
			screen2View.sliceScalingFactor = factor;
			screen2View.sliceBiasFactor = -(glm::log(camNearZ) * factor);
			screen2View.zNear = camNearZ;
			screen2View.zFar = camFarZ;
			//Generating and copying data to memory in GPU
			auto size = sizeof(struct ScreenToView);
			glBufferData(GL_SHADER_STORAGE_BUFFER, size, &screen2View, GL_STATIC_COPY);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, screenToViewSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}

		//Setting up lights buffer that contains all the lights in the scene
		{
			glGenBuffers(1, &lightSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightSSBO);

			auto tempGpuLight = GPULight();
			std::unique_ptr<std::vector<GPULight>> lightList = std::make_unique<std::vector<GPULight>>(maxLights, tempGpuLight);
			PointLight* light;
			for (unsigned int i = 0; i < pointLightCount; ++i) {
				//Fetching the light from the current scene
				light = pointLights[i].get();
				lightList.get()->at(i).position = glm::vec4(light->GetPosition(), 1.0f);
				lightList.get()->at(i).color = glm::vec4(light->GetColor(), 1.0f);
				lightList.get()->at(i).enabled = 1;
				lightList.get()->at(i).intensity = 100.0f;
				lightList.get()->at(i).range = 65.0f;
			}

			glBufferData(GL_SHADER_STORAGE_BUFFER, maxLights * sizeof(struct GPULight), &(lightList.get()->at(0)), GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, lightSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}

		//A list of indices to the lights that are active and intersect with a cluster
		{
			unsigned int totalNumLights = numClusters * maxLightsPerTile; //50 lights per tile max
			glGenBuffers(1, &lightIndexListSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightIndexListSSBO);

			//We generate the buffer but don't populate it yet.
			glBufferData(GL_SHADER_STORAGE_BUFFER, totalNumLights * sizeof(unsigned int), NULL, GL_STATIC_COPY);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, lightIndexListSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}

		//Every tile takes two unsigned ints one to represent the number of lights in that grid
		//Another to represent the offset to the light index list from where to begin reading light indexes from
		//This implementation is straight up from Olsson paper
		{
			glGenBuffers(1, &lightGridSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightGridSSBO);

			glBufferData(GL_SHADER_STORAGE_BUFFER, numClusters * 2 * sizeof(unsigned int), NULL, GL_STATIC_COPY);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, lightGridSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}

		{
			glGenBuffers(1, &visibleClusterSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, visibleClusterSSBO);

			glBufferData(GL_SHADER_STORAGE_BUFFER, numClusters * sizeof(unsigned int), NULL, GL_STATIC_COPY);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, visibleClusterSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}
	}

	void CreateClusters() 
	{
		//Building the grid of AABB enclosing the view frustum clusters
		buildAABBGridCompShader->UseShader();
		buildAABBGridCompShader->Dispatch(1, 1, gridSizeZ);
	}

	void CreateBillboard() {

		unsigned int billboardIndices[] = {
			0, 1, 2,
			2, 1, 3
		};

		GLfloat billboardVertices[] =
		{
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			-0.5f, 0.5f, 0.0f,
			0.5f, 0.5f, 0.0f,
		};

		std::shared_ptr<Billboard_Mesh> obj = std::make_shared <Billboard_Mesh>();
		obj->CreateMesh(billboardVertices, billboardIndices, 12, 6);
		billboardList.push_back(obj);

	}

	void CreateParticles()
	{
		unsigned int particlesIndices[] = {
			0, 1, 2,
			1, 3, 2
		};

		GLfloat particlesVertices[] =
		{
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			-0.5f, 0.5f, 0.0f,
			0.5f, 0.5f, 0.0f,
		};

		std::shared_ptr <ParticleSystem> obj = std::make_shared< ParticleSystem>();

		obj->CreateInstancedMesh(particlesVertices, particlesIndices, 12, 6);
		particleList.push_back(obj);
	}

	void CreateTerrain()
	{
		unsigned int terrainIndices[] = {
			0, 2, 1,
			1, 2, 3
		};

		GLfloat terrainVertices[] = {
			-terrainScaleFactor1, 0.0f,-terrainScaleFactor1,		0.0f, 0.0f,								        0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f,
			terrainScaleFactor1, 0.0f,-terrainScaleFactor1,			terrainScaleFactor1, 0.0f,						0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f,
			-terrainScaleFactor1, 0.0f, terrainScaleFactor1,		0.0f, terrainScaleFactor1,						0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f,
			terrainScaleFactor1, 0.0f,terrainScaleFactor1,			terrainScaleFactor1, terrainScaleFactor1,	    0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f
		};

		MathUtil::CalcAverageNormals(terrainIndices, 6, terrainVertices, 44, 11, 5);
		MathUtil::CalcAverageTangents(terrainIndices, 6, terrainVertices, 44, 11, 8);

		//Debug::DebugPrintReferenceTBN("ReferenceTBN", terrainVertices, 11, glm::vec3(0.0f, 1.0f, 0.0f));
		//Debug::DebugPrintTBN("TerrainTBN", terrainVertices, 5, 8);

		std::shared_ptr < Static_Mesh> obj = std::make_shared< Static_Mesh>();
		obj->CreateMeshWithTangentNormal(terrainVertices, terrainIndices, 44, 6);
		terrainList.push_back(obj);
	}

	void CreateObject() {
		unsigned int indices[] = {
			1,3,0,
			2,3,1,
			0,3,2,
			2,1,0
		};

		GLfloat vertices[] = {
			//x      y      z		u		v		nx    ny	nz      tx	  ty    tz
			-1.0f, -1.0f, -0.6f,	 0.0f, 0.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f,	//bottom left
			0.0f, -1.0f, 1.0f,		 1.0f, 0.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f,   // z axis point
			1.0f, -1.0f, -0.6f,		 0.0f, 1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f,   //bottom right
			0.0f, 1.0f, 0.0f,		 1.0f, 1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f    //top
		};

		unsigned int floorIndices[] = {
			0, 2, 1,
			1, 2, 3
		};

		GLfloat floorVertices[] = {
			-15.f, 0.0f, -15.0f,	0.0f, 0.0f,	    0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f,
			15.0f, 0.0f, -15.0f,	1.0f, 0.0f,	    0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f,
			-15.0f, 0.0f, 15.0f,	0.0f, 1.0f,	    0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f,
			15.0f, 0.0f, 15.0f,		1.0f, 1.0f,	    0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f
		};


		MathUtil::CalcAverageNormals(indices, 12, vertices, 44, 11, 5);
		MathUtil::CalcAverageTangents(indices, 12, vertices, 44, 11, 8);
		MathUtil::CalcAverageNormals(floorIndices, 6, floorVertices, 44, 11, 5);
		MathUtil::CalcAverageTangents(floorIndices, 6, floorVertices, 44, 11, 8);

		std::shared_ptr < Static_Mesh> obj1 = std::make_shared <Static_Mesh>();
		obj1->CreateMeshWithTangentNormal(vertices, indices, 44, 12);
		meshList.push_back(obj1);

		std::shared_ptr < Static_Mesh> obj2 = std::make_shared <Static_Mesh>();
		obj2->CreateMeshWithTangentNormal(vertices, indices, 44, 12);
		meshList.push_back(obj2);

		std::shared_ptr < Static_Mesh> obj3 = std::make_shared <Static_Mesh>();
		obj3->CreateMeshWithTangentNormal(floorVertices, floorIndices, 44, 6);
		meshList.push_back(obj3);

		std::shared_ptr < Static_Mesh> obj4 = std::make_shared <Static_Mesh>();
		obj4->CreateMeshWithTangentNormal(floorVertices, floorIndices, 44, 6);
		meshList.push_back(obj4);
	}

	void CreateShaders() {

		buildAABBGridCompShader->CreateFromFiles("Shaders/clusterShader.comp");
		visibleClusterCompShader->CreateFromFiles("Shaders/clusterVisibleShader.comp");
		cullLightsCompShader->CreateFromFiles("Shaders/clusterCullLightShader.comp");
		
		//ToDo: #20 simulation manager class
		//addSmokeSpotCompShader->CreateFromFiles("Shaders/2DFluid/addSmokeSpot.comp");
		//maxReduceCompShader->CreateFromFiles("Shaders/2DFluid/maxReduce.comp");
		//RKCompShader->CreateFromFiles("Shaders/2DFluid/RKAdvect.comp");
		//maccormackCompShader->CreateFromFiles("Shaders/2DFluid/maccormack.comp");
		//buoyantForceCompShader->CreateFromFiles("Shaders/2DFluid/buoyantForce.comp");
		//divRBCompShader->CreateFromFiles("Shaders/2DFluid/divRB.comp");
		//jacobiBlackCompShader->CreateFromFiles("Shaders/2DFluid/jacobiBlack.comp");
		//jacobiRedCompShader->CreateFromFiles("Shaders/2DFluid/jacobiRed.comp");
		//pressureProjectionRBCompShader->CreateFromFiles("Shaders/2DFluid/pressureProjectionRB.comp");

		//clearTexCompShader3D->CreateFromFiles("Shaders/3DFluid/clear.comp");
		//addSmokeSpotCompShader3D->CreateFromFiles("Shaders/3DFluid/addSmokeSpot.comp");
		////maxReduceCompShader->CreateFromFiles("Shaders/2DFluid/maxReduce.comp");
		//RKAdvectCompShader3D->CreateFromFiles("Shaders/3DFluid/RKAdvect.comp");
		//maccormackCompShader3D->CreateFromFiles("Shaders/3DFluid/maccormack.comp");
		//buoyantForceCompShader3D->CreateFromFiles("Shaders/3DFluid/buoyantForce.comp");
		//divCompShader3D->CreateFromFiles("Shaders/3DFluid/div.comp");
		//jacobiCompShader3D->CreateFromFiles("Shaders/3DFluid/jacobi.comp");
		//pressureProjectionCompShader3D->CreateFromFiles("Shaders/3DFluid/pressureProjection.comp");

		environmentMapShader->CreateFromFiles("Shaders/cubemap.vert", "Shaders/equirectangular_to_cubemap.frag");
		irradianceConvolutionShader->CreateFromFiles("Shaders/cubemap.vert", "Shaders/irradiance_covolution.frag");
		prefilterShader->CreateFromFiles("Shaders/cubemap.vert", "Shaders/prefilter.frag");
		brdfShader->CreateFromFiles("Shaders/framebuffer.vert", "Shaders/brdf.frag");

		directionalShadowShader->CreateFromFiles("Shaders/directional_shadow_map.vert", "Shaders/directional_shadow_map.frag");
		omniShadowShader->CreateFromFiles("Shaders/omni_shadow_map.vert", "Shaders/omni_shadow_map.geom", "Shaders/omni_shadow_map.frag");

		animDirectionalShadowShader->CreateFromFiles("Shaders/anim_directional_shadow_map.vert", "Shaders/directional_shadow_map.frag");
		animOmniShadowShader->CreateFromFiles("Shaders/anim_omni_shadow_map.vert", "Shaders/omni_shadow_map.geom", "Shaders/omni_shadow_map.frag");

		terrainDirectionalShadowShader->CreateFromFiles("Shaders/terrain.vert", "Shaders/terrain.tessc", "Shaders/terrain_directional_shadow_map.tesse", "Shaders/directional_shadow_map.frag");
		//terrainOmniDirectionalShadowShader.CreateFromFiles("Shaders/terrain.vert", "Shaders/terrain.tessc", "Shaders/terrain_omni_directional_shadow_map.tesse", "Shaders/omni_shadow_map.geom", "Shaders/directional_shadow_map.frag");

		static_preZPassShader->CreateFromFiles("Shaders/depth_framebuffer.vert", "Shaders/depth_framebuffer.frag");
		anim_preZPassShader->CreateFromFiles("Shaders/anim_depth_framebuffer.vert", "Shaders/depth_framebuffer.frag");
		terrain_preZPassShader->CreateFromFiles("Shaders/terrain.vert", "Shaders/terrain.tessc", "Shaders/terrain_depth_framebuffer.tesse", "Shaders/depth_framebuffer.frag");

		ssaoShader->CreateFromFiles("Shaders/ssao_framebuffer.vert", "Shaders/ssao_framebuffer.frag");

		ssaoBlurShader->CreateFromFiles("Shaders/framebuffer.vert", "Shaders/ssao_blur_framebuffer.frag");

		std::shared_ptr<Model_Shader> shader1 = std::make_shared<Model_Shader>();
		shader1->CreateFromFiles(vShader.c_str(), fShader.c_str(), true);
		shaderList.push_back(shader1);

		std::shared_ptr<Model_Shader> shader2 = std::make_shared < Model_Shader>();
		shader2->CreateFromFiles(avShader.c_str(), fShader.c_str());
		animShaderList.push_back(shader2);

		terrainShader->CreateFromFiles("Shaders/terrain.vert", "Shaders/terrain.tessc", "Shaders/terrain.tesse", "Shaders/terrain.frag");
		billboardShader = std::make_unique<Shader_Object>(std::vector<std::string>{"Shaders/billboard.vert", "Shaders/billboard.frag"});

		particleShader->CreateFromFiles("Shaders/particles.vert", "Shaders/particles.frag");

		hdrShader->CreateFromFiles("Shaders/framebuffer.vert", "Shaders/hdr_framebuffer.frag");

		motionBlurShader->CreateFromFiles("Shaders/framebuffer.vert", "Shaders/motionBlur_framebuffer.frag");

		blurShader->CreateFromFiles("Shaders/framebuffer.vert", "Shaders/blur_framebuffer.frag");

		//ToDo: #20 simulation manager class
		//fluidFragShader->CreateFromFiles("Shaders/framebuffer.vert", "Shaders/2DFluid/fluid.frag");
		//fluidFragShader3D->CreateFromFiles("Shaders/3DFluid/fluid.vert", "Shaders/3DFluid/fluid.frag");

		shader1 = nullptr;
		shader2 = nullptr;
	}

	void RenderBillboardScene()
	{
		billboardShader->SetVariable("BillboardPos", glm::vec3(6.0f - terrainScaleFactor, 29.0f, -terrainScaleFactor));
		billboardShader->SetVariable("BillboardSize", glm::vec2(2.0f, 2.0f/*0.125f*/));
		billboardShader->SetVariable("prevPV", camera->GetPreviousProjectionViewMatrix());

		grassTexture->UseTexture(0);
		billboardList[0]->RenderMesh();
	}

	void RenderParticlesScene(GLfloat deltaTime)
	{
		particleList[0]->GenerateParticlesCPU(deltaTime, glm::vec3(10.0f - terrainScaleFactor, 33.0f, -terrainScaleFactor));
		particleList[0]->SimulateParticlesCPU(camera->getCameraPosition(), deltaTime);
		particleList[0]->UpdateParticlesMeshCPU();
		plainTexture->UseTexture(0);
		particleList[0]->RenderInstancedMesh();
	}

	void RenderTerrain(bool shadow, bool depth)
	{
		glUniform1f(uniformDispFactor, (0.2f * terrainScaleFactor1));

		glm::mat4 model;
		glm::mat4 prevPVM = glm::mat4();
		//model = glm::translate(model, glm::vec3(0.0f,-10.0f, 0.0f));
		//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
		//model = glm::scale(model,glm::vec3(terrainScaleFactor, 1.0f, terrainScaleFactor));
		glUniformMatrix4fv(uniformModel2, 1, GL_FALSE, glm::value_ptr(model));
		prevPVM = camera->GetPreviousProjectionViewMatrix() * terrainList[0]->PrevMesh;
		glUniformMatrix4fv(uniformPrevPVM2, 1, GL_FALSE, glm::value_ptr(prevPVM));
		terrainTextureDisp->UseTexture(0);
		terrainTextureBlend->UseTexture(10);
		if (shadow)
		{
			terrainDirectionalShadowShader->SetDisplacementMap(1);
		}
		else if (depth)
		{
			terrain_preZPassShader->SetDisplacementMap(1);
		}
		else
		{
			terrainShader->SetDisplacementMap(1);
		}
		terrainShader->SetBlendMap(11);
		terrainTexture->UseTextureArray(11);
		terrainTextureMetal->UseTextureArray(12);
		terrainTextureNorm->UseTextureArray(14);
		terrainTextureRough->UseTextureArray(15);
		terrainTexturePara->UseTextureArray(16);
		dullTerrainMaterial->UseMaterial(uniformAlbedoMap2, uniformMetallicMap2, uniformNormalMap2, uniformRoughnessMap2, uniformParallaxMap2);

		terrainList[0]->RenderTessellatedMesh();
		terrainList[0]->PrevMesh = model;
	}

	void RenderEnvCubeMap(bool is_cubeMap)
	{
		if (is_cubeMap)
		{
			environmentMap->AttachFBOToTextureUnit(0, GL_TEXTURE1, 0, 0);
		}
		else
		{
			environmentTexture->UseTexture(0);
		}
		mesh_cube->RenderCube();
	}

	void RenderScene(std::shared_ptr<Shader> shader) {
		glm::mat4 model = glm::mat4();
		glm::mat4 prevPVM = glm::mat4();

		pyramid1->Translate(-terrainScaleFactor, 34.0f, -2.5f - terrainScaleFactor);
		//pyramid1->Rotate(curAngle, 0.0f, 1.0f, 0.0f);
		//pyramid1->Scale(0.4f, 0.4f, 1.0f);
		pyramid1->DrawNativeObject(shader, camera);

		pyramid2->Translate(-terrainScaleFactor, 30.0f, -2.5f - terrainScaleFactor);
		//pyramid2->Rotate(curAngle, 0.0f, 1.0f, 0.0f);
		//pyramid2->Scale(0.4f, 0.4f, 1.0f);
		pyramid2->DrawNativeObject(shader, camera);

		rectangle1->Translate(-15.0f - terrainScaleFactor, 43.0f, -terrainScaleFactor);
		rectangle1->Rotate(90, 1.0f, 0.0f, 0.0f);
		rectangle1->Rotate(-90, 0.0f, 0.0f, 1.0f);
		rectangle1->DrawNativeObject(shader, camera);

		rectangle2->Translate(-terrainScaleFactor, 43.0f, -15.0f - terrainScaleFactor);
		rectangle2->Rotate(-90, 1.0f, 0.0f, 0.0f);
		rectangle2->Rotate(180.0f, 0.0f, 0.0f, 1.0f);
		rectangle2->DrawNativeObject(shader, camera);

		cube->Translate(curScale - terrainScaleFactor, 32.0f, 4.5f - terrainScaleFactor);
		//cube->Rotate(curAngle, 0.0f, 1.0f, 0.0f);
		cube->Rotate(90.0f, 1.0f, 0.0f, 0.0f);
		cube->Scale(0.1f, 0.1f, 0.1f);
		cube->DrawImportedObject(shader, camera);

		//sphere->Translate(-terrainScaleFactor, 35.0f, 5.5f - terrainScaleFactor);
		//sphere->Rotate(curAngle, 0.0f, 1.0f, 0.0f);
		sphere->Scale(1.0f, 1.0f, 1.0f);
		sphere->DrawImportedObject(shader, camera);

		sniper->Translate(-terrainScaleFactor, 33.0f, 10.0f - terrainScaleFactor);
		//sniper->Rotate(curAngle, 0.0f, 1.0f, 0.0f);
		sniper->Rotate(90.0f, 1.0f, 0.0f, 0.0f);
		sniper->Rotate(180.0f, 1.0f, 0.0f, 0.0f);
		sniper->Scale(0.5f, 0.5f, 0.5f);
		sniper->DrawImportedObject(shader, camera);

		gun->Translate(5.0f - terrainScaleFactor, 33.0f, 10.0f - terrainScaleFactor);
		//gun->Rotate(curAngle, 0.0f, 1.0f, 0.0f);
		gun->Rotate(180.0f, 0.0f, 1.0f, 0.0f);
		gun->Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
		gun->Scale(0.02f, 0.02f, 0.02f);
		gun->DrawImportedObject(shader, camera);

		bulbWhite->Translate(pointLights[0]->GetPosition().x, pointLights[0]->GetPosition().y, pointLights[0]->GetPosition().z);
		bulbWhite->Scale(10.0f, 10.f, 10.0f);
		bulbWhite->DrawImportedObject(shader, camera);

		bulbRed->Translate(pointLights[1]->GetPosition().x, pointLights[1]->GetPosition().y, pointLights[1]->GetPosition().z);
		bulbRed->Scale(10.0f, 10.f, 10.0f);
		bulbRed->DrawImportedObject(shader, camera);

		anymodel->Translate(-terrainScaleFactor, 37.0f, 1.0f - terrainScaleFactor);
		anymodel->Scale(1.0f, 1.0f, 1.0f);
		anymodel->DrawImportedObject(shader, camera);
	}

	void RenderAnimScene(bool shadow, bool depth) {
		glm::mat4 model;
		glm::mat4 prevPVM = glm::mat4();

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-6.0f - terrainScaleFactor, 28.2f, -5.0f - terrainScaleFactor));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
		model = glm::scale(model, glm::vec3(0.1, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel1, 1, GL_FALSE, glm::value_ptr(model));
		prevPVM = camera->GetPreviousProjectionViewMatrix() * anim->prevModel;
		glUniformMatrix4fv(uniformPrevPVM1, 1, GL_FALSE, glm::value_ptr(prevPVM));
		shinyMaterialGlow->UseMaterial(uniformAlbedoMap1, uniformMetallicMap1, uniformNormalMap1, uniformRoughnessMap1, uniformParallaxMap1, uniformGlowMap1);

		if (shadow)
		{
			anim->UpdateBoneData(animDirectionalShadowShader->GetShaderID());
		}
		else if (depth)
		{
			anim->UpdateBoneData(anim_preZPassShader->GetShaderID());
		}
		else {
			anim->UpdateBoneData(animShaderList[0]->GetShaderID());
		}
		anim->RenderModel();
		anim->prevModel = model;

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(6.0f - terrainScaleFactor, 28.2f, -5.0f - terrainScaleFactor));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel1, 1, GL_FALSE, glm::value_ptr(model));
		prevPVM = camera->GetPreviousProjectionViewMatrix() * anim2->prevModel;
		glUniformMatrix4fv(uniformPrevPVM1, 1, GL_FALSE, glm::value_ptr(prevPVM));
		shinyMaterialGlow->UseMaterial(uniformAlbedoMap1, uniformMetallicMap1, uniformNormalMap1, uniformRoughnessMap1, uniformParallaxMap1, uniformGlowMap1);

		if (shadow)
		{
			anim2->UpdateBoneData(animDirectionalShadowShader->GetShaderID());
		}
		else if (depth)
		{
			anim2->UpdateBoneData(anim_preZPassShader->GetShaderID());
		}
		else
		{
			anim2->UpdateBoneData(animShaderList[0]->GetShaderID());
		}

		anim2->RenderModel();
		anim2->prevModel = model;
	}

	void EnvironmentMapPass()
	{
		environmentMapShader->UseShader();
		environmentMapShader->SetTexture(1);

		glUniformMatrix4fv(environmentMapShader->GetProjectionLocation(), 1, GL_FALSE, glm::value_ptr(captureProjection));

		glViewport(0, 0, environmentMap->GetFBOWidth(), environmentMap->GetFBOHeight());
		environmentMap->BindFBO();
		for (auto faceId = 0; faceId < 6; ++faceId)
		{
			glUniformMatrix4fv(environmentMapShader->GetViewLocation(), 1, GL_FALSE, glm::value_ptr(captureViews[faceId]));
			environmentMap->WriteToFBOBuffer(0, 0, 0, faceId);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			environmentMapShader->Validate();

			RenderEnvCubeMap(false);
		}
		environmentMap->CreateFBOMipMap(0, 0, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void IrradianceConvolutionPass()
	{
		irradianceConvolutionShader->UseShader();
		irradianceConvolutionShader->SetSkybox(1);

		glUniformMatrix4fv(irradianceConvolutionShader->GetProjectionLocation(), 1, GL_FALSE, glm::value_ptr(captureProjection));

		glViewport(0, 0, irradianceMap->GetFBOWidth(), irradianceMap->GetFBOHeight());
		irradianceMap->BindFBO();
		for (unsigned int i = 0; i < 6; ++i)
		{
			glUniformMatrix4fv(irradianceConvolutionShader->GetViewLocation(), 1, GL_FALSE, glm::value_ptr(captureViews[i]));
			irradianceMap->WriteToFBOBuffer(0, 0, 0, i);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			irradianceConvolutionShader->Validate();

			RenderEnvCubeMap(true);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void PrefilterPass()
	{
		prefilterShader->UseShader();
		prefilterShader->SetSkybox(1);
		glUniformMatrix4fv(prefilterShader->GetProjectionLocation(), 1, GL_FALSE, glm::value_ptr(captureProjection));

		prefilterMap->BindFBO();
		unsigned int maxMipLevels = 5;

		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			//resize framebuffer according to mip-level size
			unsigned int mipWidth = prefilterMap->GetFBOWidth() * std::pow(0.5, mip);
			unsigned int mipHeight = prefilterMap->GetFBOHeight() * std::pow(0.5, mip);
			//prefilterMap->Write(-1, mipWidth, mipHeight, 0);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			prefilterShader->SetRoughness(roughness);
			for (auto faceId = 0; faceId < 6; ++faceId)
			{
				glUniformMatrix4fv(prefilterShader->GetViewLocation(), 1, GL_FALSE, glm::value_ptr(captureViews[faceId]));
				prefilterMap->WriteToFBOBuffer(0, 0, 0, faceId, mip);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				prefilterShader->Validate();

				RenderEnvCubeMap(true);
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void BRDFPass()
	{
		glViewport(0, 0, brdfMap->GetFBOWidth(), brdfMap->GetFBOHeight());

		brdfMap->BindFBO();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		brdfShader->UseShader();
		quad->RenderQuad();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void DirectionalShadowMapPass(DirectionalLight* light) {

		testLitView[0] = light->CalculateCascadeLightTransform();
		mainLight->CalcOrthProjs(camera->CalculateViewMatrix(), testLitView, 60.0f);

		for (unsigned int i = 0; i < NUM_CASCADES; ++i)
		{
			vView[i] = glm::lookAt(mainLight->GetModlCent(i), mainLight->GetModlCent(i) + glm::normalize(light->GetLightDirection()) * 0.2f, light->GetLightUp());
		}

		light->GetShadowMap()->BindFBO();

		for (size_t i = 0; i < NUM_CASCADES; ++i)
		{
			glViewport(0, 0, light->GetShadowMap()->GetFBOWidth(), light->GetShadowMap()->GetFBOHeight());
			light->GetShadowMap()->WriteToFBOBuffer(0, 0, i, 0);
			glClear(GL_DEPTH_BUFFER_BIT);

			glm::mat4 projView = light->GetProjMat(vView[i], i) * vView[i];

			directionalShadowShader->UseShader();
			directionalShadowShader->SetDirectionalLightTransform(projView);
			directionalShadowShader->Validate();

			RenderScene(directionalShadowShader);


			animDirectionalShadowShader->UseShader();

			uniformModel1 = animDirectionalShadowShader->GetModelLocation();

			animDirectionalShadowShader->SetDirectionalLightTransform(projView);

			animDirectionalShadowShader->Validate();

			RenderAnimScene(true, false);

			terrainDirectionalShadowShader->UseShader();
			uniformModel2 = terrainDirectionalShadowShader->GetModelLocation();
			uniformEyePosition2 = terrainDirectionalShadowShader->GetEyePositionLocation();
			uniformDispFactor = terrainDirectionalShadowShader->GetDispFactorLocation();

			terrainDirectionalShadowShader->SetDirectionalLightTransform(&projView);
			glUniform3f(uniformEyePosition2, camera->getCameraPosition().x, camera->getCameraPosition().y, camera->getCameraPosition().z);

			terrainDirectionalShadowShader->Validate();

			RenderTerrain(true, false);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OmniShadowMapPass(PointLight* light, int lightIndex) {

		omniShadowShader->UseShader();

		glViewport(0, 0, light->GetShadowMap()->GetFBOWidth(lightIndex), light->GetShadowMap()->GetFBOHeight(lightIndex));

		light->GetShadowMap()->BindFBO(lightIndex);
		glClear(GL_DEPTH_BUFFER_BIT);

		glUniform3f(omniShadowShader->GetOmniLightPosLocation(), light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);
		glUniform1f(omniShadowShader->GetFarPlaneLocation(), light->GetFarPlane());
		omniShadowShader->SetLightMatrices(light->CalculateLightTransform());

		omniShadowShader->Validate();
		RenderScene(omniShadowShader);

		animOmniShadowShader->UseShader();

		uniformModel1 = animOmniShadowShader->GetModelLocation();
		uniformOmniLightPos1 = animOmniShadowShader->GetOmniLightPosLocation();
		uniformFarPlane1 = animOmniShadowShader->GetFarPlaneLocation();

		glUniform3f(uniformOmniLightPos1, light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);
		glUniform1f(uniformFarPlane1, light->GetFarPlane());
		animOmniShadowShader->SetLightMatrices(light->CalculateLightTransform());

		animOmniShadowShader->Validate();

		RenderAnimScene(true, false);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void CullLight()
	{
		visibleClusterCompShader->UseShader();
		depth->AttachFBOToTextureUnit(0, GL_TEXTURE0, 0, 0);
		visibleClusterCompShader->Dispatch(ScreenWidth / 32, ScreenHeight / 30, 1);

		//unsigned int count = 0;
		//unsigned int isvisible[32 * 32 * 10];

		//glGetNamedBufferSubData(visibleClusterSSBO, 0, numClusters * sizeof(unsigned int), &isvisible);
		//for(int i = 0; i < numClusters; ++i)
		//{
		//	if (isvisible[i]) ++count;
		//}

		//4-Light assignment
		cullLightsCompShader->UseShader();
		glUniformMatrix4fv(cullLightsCompShader->GetViewLocation(), 1, GL_FALSE, glm::value_ptr(camera->CalculateViewMatrix()));
		cullLightsCompShader->Dispatch(1, 1, gridSizeZ);

		//unsigned int count1 = 0;
		//unsigned int grid[32 * 32 * 10 * 2];

		//glGetNamedBufferSubData(lightGridSSBO, 0, numClusters * 2 * sizeof(unsigned int), &grid);
		//for (int i = 0; i < numClusters * 2; i += 2)
		//{
		//	if (grid[i + 1]) ++count1;
		//}

		//if (count1 == count)
		//	std::cout << count << " Clusters are Active" << std::endl;
	}

	void PreZPass(GLfloat deltaTime)
	{
		auto projectionMatrix = camera->GetProjectionMatrix();
		auto viewMatrix = camera->CalculateViewMatrix();
		glViewport(0, 0, depth->GetFBOWidth(), depth->GetFBOHeight());

		depth->BindFBO();
		//clear everything
		glClear(GL_DEPTH_BUFFER_BIT);

		terrain_preZPassShader->UseShader();
		uniformModel2 = terrain_preZPassShader->GetModelLocation();
		uniformProjection2 = terrain_preZPassShader->GetProjectionLocation();
		uniformView2 = terrain_preZPassShader->GetViewLocation();
		uniformEyePosition2 = terrain_preZPassShader->GetEyePositionLocation();
		uniformDispFactor = terrain_preZPassShader->GetDispFactorLocation();

		glUniformMatrix4fv(uniformProjection2, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glUniformMatrix4fv(uniformView2, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniform3f(uniformEyePosition2, camera->getCameraPosition().x, camera->getCameraPosition().y, camera->getCameraPosition().z);

		terrain_preZPassShader->Validate();

		RenderTerrain(false, true);

		static_preZPassShader->UseShader();
		static_preZPassShader->Validate();
		RenderScene(static_preZPassShader);

		anim_preZPassShader->UseShader();
		uniformModel1 = anim_preZPassShader->GetModelLocation();
		uniformProjection1 = anim_preZPassShader->GetProjectionLocation();
		uniformView1 = anim_preZPassShader->GetViewLocation();

		glUniformMatrix4fv(uniformProjection1, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glUniformMatrix4fv(uniformView1, 1, GL_FALSE, glm::value_ptr(viewMatrix));

		anim_preZPassShader->Validate();

		RenderAnimScene(false, true);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void SSAOPass()
	{
		glViewport(0, 0, ssao->GetFBOWidth(), ssao->GetFBOHeight());
		ssao->BindFBO();
		glClear(GL_COLOR_BUFFER_BIT);

		ssaoShader->UseShader();

		depth->AttachFBOToTextureUnit(0, GL_TEXTURE1, 0, 0);
		ssaoShader->SetTexture(1);

		SSAONoiseTexture->UseTexture(1);
		ssaoShader->SetNoiseTexture(2);

		glUniform1f(ssaoShader->GetSampleRadiusLocation(), 0.1f);
		glUniformMatrix4fv(ssaoShader->GetProjectionLocation(), 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

		quad->RenderQuad();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void SSAOBlurPass()
	{
		glViewport(0, 0, ssaoBlur->GetFBOWidth(), ssaoBlur->GetFBOHeight());

		ssaoBlur->BindFBO();
		glClear(GL_COLOR_BUFFER_BIT);

		ssaoBlurShader->UseShader();
		ssao->AttachFBOToTextureUnit(0, GL_TEXTURE1, 0, 0);
		ssaoBlurShader->SetTexture(1);

		quad->RenderQuad();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void RenderPass(GLfloat deltaTime)
	{
		auto projectionMatrix = camera->GetProjectionMatrix();
		auto viewMatrix = camera->CalculateViewMatrix();
		auto prevProj = camera->GetPreviousProjectionMatrix();
		auto prevView = camera->GetPreviousViewMatrix();

		glViewport(0, 0, hdr->GetFBOWidth(), hdr->GetFBOHeight());

		hdr->BindFBO();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		irradianceMap->AttachFBOToTextureUnit(0, GL_TEXTURE8, 0, 0);
		prefilterMap->AttachFBOToTextureUnit(0, GL_TEXTURE9, 0, 0);
		brdfMap->AttachFBOToTextureUnit(0, GL_TEXTURE10, 0, 0);
		ssaoBlur->AttachFBOToTextureUnit(0, GL_TEXTURE14, 0, 0);
		depth->AttachFBOToTextureUnit(0, GL_TEXTURE18, 0, 0);

		skybox->DrawHDRSkybox(viewMatrix, projectionMatrix, prevProj, prevView, environmentMap); //should be at the end to prevent overdraw,here becoz of blending issues

		terrainShader->UseShader();

		uniformModel2 = terrainShader->GetModelLocation();
		uniformProjection2 = terrainShader->GetProjectionLocation();
		uniformView2 = terrainShader->GetViewLocation();
		uniformPrevPVM2 = terrainShader->GetPrevPVMLocation();
		uniformEyePosition2 = terrainShader->GetEyePositionLocation();
		uniformHeightScale2 = terrainShader->GetHeightScaleLocation();
		uniformDispFactor = terrainShader->GetDispFactorLocation();
		uniformAlbedoMap2 = terrainShader->GetAlbedoLocation();
		uniformMetallicMap2 = terrainShader->GetMetallicLocation();
		uniformNormalMap2 = terrainShader->GetNormalLocation();
		uniformRoughnessMap2 = terrainShader->GetRoughnessLocation();
		uniformParallaxMap2 = terrainShader->GetParallaxLocation();

		glUniformMatrix4fv(uniformProjection2, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glUniformMatrix4fv(uniformView2, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniform3f(uniformEyePosition2, camera->getCameraPosition().x, camera->getCameraPosition().y, camera->getCameraPosition().z);
		glUniform1f(uniformHeightScale2, 0.02f);

		terrainShader->SetDirectionalLight(mainLight.get());
		terrainShader->SetPointLight(pointLights, pointLightCount, 5, 0);
		terrainShader->SetSpotLight(spotLights, spotLightCount, 5 + pointLightCount, pointLightCount);

		for (size_t i = 0; i < NUM_CASCADES; ++i)
		{
			glm::mat4 projView = mainLight->GetProjMat(vView[i], i) * vView[i];
			terrainShader->SetDirectionalLightTransforms(i, &projView);
		}

		terrainShader->SetDirectionalShadowMaps(mainLight.get(), NUM_CASCADES, 2);
		terrainShader->SetAOMap(14);
		terrainShader->SetDepthMap(18);
		terrainShader->SetIrradianceMap(8);
		terrainShader->SetPrefilterMap(9);
		terrainShader->SetBRDFLUT(10);

		terrainShader->Validate();

		RenderTerrain(false, false);

		shaderList[0]->UseShader();

		shaderList[0]->SetDirectionalLight(mainLight.get());
		shaderList[0]->SetPointLight(pointLights, pointLightCount, 3, 0);
		shaderList[0]->SetSpotLight(spotLights, spotLightCount, 3 + pointLightCount, pointLightCount);
		
		for (size_t i = 0; i < NUM_CASCADES; ++i)
		{
			glm::mat4 projView = mainLight->GetProjMat(vView[i], i) * vView[i];
			shaderList[0]->SetDirectionalLightTransforms(i, &projView);
		}

		shaderList[0]->SetDirectionalShadowMaps(mainLight.get(), NUM_CASCADES, 19);
		shaderList[0]->Validate();
		RenderScene(shaderList[0]);

		animShaderList[0]->UseShader();

		uniformModel1 = animShaderList[0]->GetModelLocation();
		uniformProjection1 = animShaderList[0]->GetProjectionLocation();
		uniformView1 = animShaderList[0]->GetViewLocation();
		uniformPrevPVM1 = animShaderList[0]->GetPrevPVMLocation();
		uniformEyePosition1 = animShaderList[0]->GetEyePositionLocation();
		uniformHeightScale1 = animShaderList[0]->GetHeightScaleLocation();
		uniformAlbedoMap1 = animShaderList[0]->GetAlbedoLocation();
		uniformMetallicMap1 = animShaderList[0]->GetMetallicLocation();
		uniformNormalMap1 = animShaderList[0]->GetNormalLocation();
		uniformRoughnessMap1 = animShaderList[0]->GetRoughnessLocation();
		uniformParallaxMap1 = animShaderList[0]->GetParallaxLocation();
		uniformGlowMap1 = animShaderList[0]->GetGlowLocation();

		glUniformMatrix4fv(uniformProjection1, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glUniformMatrix4fv(uniformView1, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniform3f(uniformEyePosition1, camera->getCameraPosition().x, camera->getCameraPosition().y, camera->getCameraPosition().z);

		glUniform1f(uniformHeightScale1, 0.0f);

		animShaderList[0]->SetDirectionalLight(mainLight.get());
		animShaderList[0]->SetPointLight(pointLights, pointLightCount, 3, 0);
		animShaderList[0]->SetSpotLight(spotLights, spotLightCount, 3 + pointLightCount, pointLightCount);
		
		for (size_t i = 0; i < NUM_CASCADES; ++i)
		{
			glm::mat4 projView = mainLight->GetProjMat(vView[i], i) * vView[i];
			animShaderList[0]->SetDirectionalLightTransforms(i, &projView);
		}

		animShaderList[0]->SetDirectionalShadowMaps(mainLight.get(), NUM_CASCADES, 19);
		animShaderList[0]->SetAOMap(14);
		animShaderList[0]->SetDepthMap(18);
		animShaderList[0]->SetIrradianceMap(8);
		animShaderList[0]->SetPrefilterMap(9);
		animShaderList[0]->SetBRDFLUT(10);

		animShaderList[0]->Validate();

		RenderAnimScene(false, false);

		billboardShader->UseShaderObject();

		billboardShader->SetVariable("Projection", projectionMatrix);
		billboardShader->SetVariable("View", viewMatrix);
		billboardShader->SetVariable("CameraUp_worldspace", camera->getCameraUp());
		billboardShader->SetVariable("CameraRight_worldspace", camera->getCameraRight());
		billboardShader->SetVariable("theTexture", 1);

		billboardShader->ValidateShaderObject();

		RenderBillboardScene();

		particleShader->UseShader();

		glUniformMatrix4fv(particleShader->GetProjectionLocation(), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glUniformMatrix4fv(particleShader->GetViewLocation(), 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniform3f(particleShader->GetCameraUpLocation(), camera->getCameraUp().x, camera->getCameraUp().y, camera->getCameraUp().z);
		glUniform3f(particleShader->GetCameraRightLocation(), camera->getCameraRight().x, camera->getCameraRight().y, camera->getCameraRight().z);

		particleShader->SetTexture(1);

		particleShader->Validate();

		RenderParticlesScene(deltaTime);

		//ToDo: #20 simulation manager class
		//Render3DSmoke();

		glm::vec3 lowerLight = camera->getCameraPosition();
		lowerLight.y -= 0.1f;
		spotLights[0]->SetFlash(lowerLight, camera->getCameraDirection());

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Bloom()
	{
		bool isHorizontalFbo = true;
		int amount = 10;
		blurShader->UseShader();
		for (int i = 0; i < amount; i++)
		{
			blur->BindFBO(isHorizontalFbo);
			glUniform1i(blurShader->GetHorizontalLocation(), isHorizontalFbo);
			blurShader->Validate();
			blurShader->SetTexture(1);
			if (i < 1)
			{
				hdr->AttachFBOToTextureUnit(0, GL_TEXTURE1,0, 1);
			}
			else
			{
				blur->AttachFBOToTextureUnit(!isHorizontalFbo, GL_TEXTURE1, 0, 0);
			}
			quad->RenderQuad();
			isHorizontalFbo = !isHorizontalFbo;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void MotionBlurPass(float fps)
	{
		glViewport(0, 0, motionBlur->GetFBOWidth(), motionBlur->GetFBOHeight());

		motionBlur->BindFBO();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		motionBlurShader->UseShader();

		glUniform1f(motionBlurShader->GetVelocityScaleLocation(), fps / 30.0f);

		hdr->AttachFBOToTextureUnit(0, GL_TEXTURE1, 0, 0);
		motionBlurShader->SetTexture(1);

		hdr->AttachFBOToTextureUnit(0, GL_TEXTURE2, 1, 2);
		motionBlurShader->SetMotionTexture(2);
		quad->RenderQuad();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//ToDo: #20 simulation manager class
	//float rd() { return (float)rand() / (float)RAND_MAX; }

	//void addSplatSpot(glm::vec2 spotPos, glm::vec3 color, float intensity, Texture* tex, float readTexId)
	//{
	//	addSmokeSpotCompShader->UseShader();
	//	auto addSmokeSpotProgram = addSmokeSpotCompShader->GetShaderID();
	//	GLuint location = glGetUniformLocation(addSmokeSpotProgram, "spotPos");
	//	glUniform2i(location, spotPos.x, spotPos.y);
	//	location = glGetUniformLocation(addSmokeSpotProgram, "color");
	//	glUniform3f(location, color.x, color.y, color.z);
	//	location = glGetUniformLocation(addSmokeSpotProgram, "intensity");
	//	glUniform1f(location, intensity);
	//	location = glGetUniformLocation(addSmokeSpotProgram, "texIndex");
	//	glUniform1i(location, readTexId);

	//	tex->UseTextureReadWrite(0, false, true);
	//	addSmokeSpotCompShader->Dispatch(simWidth / 32, simHeight / 32, 1);
	//}

	//void addSplatSpot3D(glm::vec3 spotPos, glm::vec3 color, float intensity, float radius, Texture* tex)
	//{
	//	addSmokeSpotCompShader3D->UseShader();
	//	auto addSmokeSpotProgram = addSmokeSpotCompShader3D->GetShaderID();
	//	GLuint location = glGetUniformLocation(addSmokeSpotProgram, "spotPos");
	//	glUniform3i(location, spotPos.x, spotPos.y, spotPos.z);
	//	location = glGetUniformLocation(addSmokeSpotProgram, "color");
	//	glUniform3f(location, color.x, color.y, color.z);
	//	location = glGetUniformLocation(addSmokeSpotProgram, "intensity");
	//	glUniform1f(location, intensity);
	//	location = glGetUniformLocation(addSmokeSpotProgram, "radius");
	//	glUniform1f(location, radius);

	//	tex->UseTextureReadWrite(0, false, true);
	//	addSmokeSpotCompShader3D->Dispatch(simDim3D / 32, simDim3D / 32, simDim3D);
	//}

	//float maxReduce(const int inTexId, Texture* texture, Texture* finalReduceTexture)
	//{
	//	maxReduceCompShader->UseShader();
	//	finalReduceTexture->UseTextureReadWrite(0, false, false);
	//	texture->UseTextureArray(0);
	//	GLuint location = glGetUniformLocation(maxReduceCompShader->GetShaderID(), "inTexIndex");
	//	glUniform1i(location, inTexId);
	//	maxReduceCompShader->Dispatch(simWidth / 32, simHeight / 32, 1);

	//	float size = 4 * (simWidth / 32) * (simHeight / 32);

	//	float* data = new float[size];
	//	finalReduceTexture->GetTextureData(data);

	//	float m = abs(data[0]);

	//	for (size_t j = 0; j < size; ++j) {
	//		if (abs(data[j]) > m) {
	//			m = abs(data[j]);
	//		}
	//	}

	//	delete[] data;

	//	return m;
	//}

	//void RKAdvect(Texture* velocityTex, Texture* fieldst, const int readVelId, const int readId, const int writeId, const float dt)
	//{
	//	RKCompShader->UseShader();
	//	GLuint location = glGetUniformLocation(RKCompShader->GetShaderID(), "dt");
	//	glUniform1f(location, dt);
	//	location = glGetUniformLocation(RKCompShader->GetShaderID(), "readFieldIndex");
	//	glUniform1i(location, readId);
	//	location = glGetUniformLocation(RKCompShader->GetShaderID(), "writeFieldIndex");
	//	glUniform1i(location, writeId);
	//	location = glGetUniformLocation(RKCompShader->GetShaderID(), "readVelocityIndex");
	//	glUniform1i(location, readVelId);
	//	fieldst->UseTextureReadWrite(0, false, true);
	//	velocityTex->UseTextureArray(0);

	//	RKCompShader->Dispatch(simWidth / 32, simHeight / 32, 1);
	//}

	//void RKAdvect3D(Texture* velocityTex, Texture* field_R, Texture* field_W, const float dt)
	//{
	//	RKAdvectCompShader3D->UseShader();
	//	GLuint location = glGetUniformLocation(RKAdvectCompShader3D->GetShaderID(), "dt");
	//	glUniform1f(location, dt);
	//	field_W->UseTextureReadWrite(0, true, true);
	//	field_R->UseTexture3D(0);
	//	velocityTex->UseTexture3D(1);

	//	RKAdvectCompShader3D->Dispatch(simDim3D / 32, simDim3D / 32, simDim3D);
	//}

	//void mcAdvect(Texture* velocityTex, Texture* fieldst, const int readVelId, int* texId)
	//{
	//	RKAdvect(velocityTex, fieldst, readVelId, texId[0], texId[1], simDt);
	//	RKAdvect(velocityTex, fieldst, readVelId, texId[1], texId[2], -simDt);
	//	maccormackStep(texId[3], texId[0], texId[1], texId[2], readVelId, velocityTex, fieldst);
	//}

	//void mcAdvect3D(Texture* velocityTex, std::vector<std::unique_ptr<Texture>>* field)
	//{
	//	RKAdvect3D(velocityTex, field->at(0).get(), field->at(1).get(), simDt3D);
	//	RKAdvect3D(velocityTex, field->at(1).get(), field->at(2).get(), -simDt3D);
	//	maccormackStep3D(field->at(3).get(), field->at(0).get(), field->at(1).get(), field->at(2).get(), velocityTex, simDt3D);
	//}

	//void maccormackStep(const int field_WRITE, const int field_n, const int field_n_1, const int field_n_hat, const int readVelId, Texture* velocityTex, Texture* fieldst)
	//{
	//	maccormackCompShader->UseShader();
	//	GLuint location = glGetUniformLocation(maccormackCompShader->GetShaderID(), "dt");
	//	glUniform1f(location, simDt);
	//	location = glGetUniformLocation(maccormackCompShader->GetShaderID(), "revert");
	//	glUniform1f(location, 0.05f);
	//	location = glGetUniformLocation(maccormackCompShader->GetShaderID(), "field_WRITE");
	//	glUniform1i(location, field_WRITE);
	//	location = glGetUniformLocation(maccormackCompShader->GetShaderID(), "field_n");
	//	glUniform1i(location, field_n);
	//	location = glGetUniformLocation(maccormackCompShader->GetShaderID(), "field_n_hat_READ");
	//	glUniform1i(location, field_n_hat);
	//	location = glGetUniformLocation(maccormackCompShader->GetShaderID(), "field_n_1_READ");
	//	glUniform1i(location, field_n_1);
	//	location = glGetUniformLocation(maccormackCompShader->GetShaderID(), "readVelocityIndex");
	//	glUniform1i(location, readVelId);

	//	fieldst->UseTextureReadWrite(0, false, true);
	//	velocityTex->UseTextureArray(0);
	//	maccormackCompShader->Dispatch(simWidth / 32, simHeight / 32, 1);
	//}

	//void maccormackStep3D(Texture* field_WRITE, Texture* field_n, Texture* field_n_1, Texture* field_n_hat, Texture* velocityTex, float dt)
	//{
	//	maccormackCompShader3D->UseShader();
	//	GLuint location = glGetUniformLocation(maccormackCompShader3D->GetShaderID(), "dt");
	//	glUniform1f(location, dt);
	//	location = glGetUniformLocation(maccormackCompShader3D->GetShaderID(), "revert");
	//	glUniform1f(location, 0.05f);

	//	field_WRITE->UseTextureReadWrite(0, true, true);
	//	field_n->UseTexture3D(0);
	//	field_n_hat->UseTexture3D(1);
	//	field_n_1->UseTexture3D(2);
	//	velocityTex->UseTexture3D(3);
	//	maccormackCompShader3D->Dispatch(simDim3D / 32, simDim3D / 32, simDim3D);
	//}

	//void applyBuoyantForce(Texture* velocity, int velocities_READ_WRITE, Texture* temperature, int temperature_READ, Texture* density, int density_READ, float kappa, float sigma, float t0)
	//{
	//	buoyantForceCompShader->UseShader();
	//	GLuint location = glGetUniformLocation(buoyantForceCompShader->GetShaderID(), "dt");
	//	glUniform1f(location, simDt);
	//	location = glGetUniformLocation(buoyantForceCompShader->GetShaderID(), "kappa");
	//	glUniform1f(location, kappa);
	//	location = glGetUniformLocation(buoyantForceCompShader->GetShaderID(), "sigma");
	//	glUniform1f(location, sigma);
	//	location = glGetUniformLocation(buoyantForceCompShader->GetShaderID(), "t0");
	//	glUniform1f(location, t0);
	//	location = glGetUniformLocation(buoyantForceCompShader->GetShaderID(), "velocities_READ_WRITE");
	//	glUniform1i(location, velocities_READ_WRITE);
	//	location = glGetUniformLocation(buoyantForceCompShader->GetShaderID(), "temperature_READ");
	//	glUniform1i(location, temperature_READ);
	//	location = glGetUniformLocation(buoyantForceCompShader->GetShaderID(), "density_READ");
	//	glUniform1i(location, density_READ);
	//	velocity->UseTextureReadWrite(0, false, true);
	//	temperature->UseTextureArray(0);
	//	density->UseTextureArray(1);

	//	buoyantForceCompShader->Dispatch(simWidth / 32, simHeight / 32, 1);
	//}

	//void applyBuoyantForce3D(Texture* velocity, Texture* temperature_READ, Texture* density_READ, float kappa, float sigma, float t0)
	//{
	//	buoyantForceCompShader3D->UseShader();
	//	GLuint location = glGetUniformLocation(buoyantForceCompShader3D->GetShaderID(), "dt");
	//	glUniform1f(location, simDt3D);
	//	location = glGetUniformLocation(buoyantForceCompShader3D->GetShaderID(), "kappa");
	//	glUniform1f(location, kappa);
	//	location = glGetUniformLocation(buoyantForceCompShader3D->GetShaderID(), "sigma");
	//	glUniform1f(location, sigma);
	//	location = glGetUniformLocation(buoyantForceCompShader3D->GetShaderID(), "t0");
	//	glUniform1f(location, t0);
	//	velocity->UseTextureReadWrite(0, false, true);
	//	temperature_READ->UseTexture3D(0);
	//	density_READ->UseTexture3D(1);

	//	buoyantForceCompShader3D->Dispatch(simDim3D / 32, simDim3D / 32, simDim3D);
	//}

	//void RBMethod(Texture* velocities, Texture* divergence, Texture* pressure, int velocityRead, int velocityWrite)
	//{
	//	divRBCompShader->UseShader();
	//	divergence->UseTextureReadWrite(0, false, false);
	//	velocities->UseTextureArray(0);
	//	GLuint location = glGetUniformLocation(divRBCompShader->GetShaderID(), "velocities_READ");
	//	glUniform1i(location, velocityRead);

	//	divRBCompShader->Dispatch(simWidth /32 / 2, simHeight /32 / 2, 1);

	//	for (unsigned i = 0; i < 50; ++i)
	//	{
	//		jacobiBlackCompShader->UseShader();
	//		pressure->UseTextureReadWrite(0, false, false);
	//		pressure->UseTexture(0);
	//		divergence->UseTexture(1);
	//		jacobiBlackCompShader->Dispatch(simWidth / 32 / 2, simHeight / 32 / 2, 1);

	//		jacobiRedCompShader->UseShader();
	//		pressure->UseTextureReadWrite(0, false, false);
	//		pressure->UseTexture(0);
	//		divergence->UseTexture(1);
	//		jacobiRedCompShader->Dispatch(simWidth / 32 / 2, simHeight / 32 / 2, 1);
	//	}

	//	pressureProjectionRBCompShader->UseShader();
	//	velocities->UseTextureReadWrite(0, false, true);
	//	pressure->UseTexture(0);
	//	location = glGetUniformLocation(pressureProjectionRBCompShader->GetShaderID(), "velocities_WRITE");
	//	glUniform1i(location, velocityWrite);
	//	location = glGetUniformLocation(pressureProjectionRBCompShader->GetShaderID(), "velocities_READ");
	//	glUniform1i(location, velocityRead);
	//	pressureProjectionRBCompShader->Dispatch(simWidth / 32 / 2, simHeight / 32 / 2, 1);
	//}

	//void RBMethod3D(Texture* velocityRead, Texture* velocityWrite, Texture* divergence, Texture* pressure)
	//{
	//	divCompShader3D->UseShader();
	//	divergence->UseTextureReadWrite(0, true, true);
	//	velocityRead->UseTextureReadWrite(1, false, true);

	//	divCompShader3D->Dispatch(simDim3D / 32, simDim3D / 32, simDim3D);

	//	for (unsigned i = 0; i < 50; ++i)
	//	{
	//		jacobiCompShader3D->UseShader();
	//		pressure->UseTextureReadWrite(0, false, true);
	//		divergence->UseTextureReadWrite(1, false, true);
	//		jacobiCompShader3D->Dispatch(simDim3D / 32, simDim3D / 32, simDim3D);
	//	}

	//	pressureProjectionCompShader3D->UseShader();
	//	velocityWrite->UseTextureReadWrite(0, true, true);
	//	velocityRead->UseTextureReadWrite(1, false, true);
	//	pressure->UseTextureReadWrite(2, false, true);
	//	pressureProjectionCompShader3D->Dispatch(simDim3D / 32, simDim3D / 32, simDim3D);
	//}

	//void Render2DFluidSim()
	//{
	//	glViewport(0, 0, ScreenWidth, ScreenHeight);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//	if (addSplat)
	//	{
	//		float vScale = 1.0f;
	//		auto cursorPos = mainWindow->GetCursorPos();
	//		cursorPos.x = (double)simWidth * cursorPos.x / (double)ScreenWidth;
	//		cursorPos.y = (double)simHeight * (1.0 - cursorPos.y / (double)ScreenHeight);

	//		addSplatSpot(cursorPos, glm::vec3(vScale * (cursorPos.x - sOriginX), vScale * (cursorPos.y - sOriginY), 0.0f), 40.0f, velocitiesTexture.get(), velTexId[0]);

	//		sOriginX = cursorPos.x;
	//		sOriginY = cursorPos.y;

	//		addSplatSpot(cursorPos, glm::vec3(rd(), rd(), rd()), 1.0f, density.get(), denTexId[0]);
	//	}

	//	float vMax = maxReduce(velTexId[0], velocitiesTexture.get(), finalReduceTexture.get());
	//	if (vMax > 1e-10f) simDt = 5.0f / vMax;

	//	mcAdvect(velocitiesTexture.get(), velocitiesTexture.get(), velTexId[0], velTexId);
	//	std::swap(velTexId[0], velTexId[3]);

	//	mcAdvect(velocitiesTexture.get(), density.get(), velTexId[0], denTexId);
	//	std::swap(denTexId[0], denTexId[3]);

	//	RBMethod(velocitiesTexture.get(), divRBTexture.get(), pressureRBTexture.get(), velTexId[0], velTexId[1]);
	//	std::swap(velTexId[0], velTexId[1]);

	//	fluidFragShader->UseShader();
	//	density->UseTextureArray(0);
	//	glUniform1i(glGetUniformLocation(fluidFragShader->GetShaderID(), "texIndex"), denTexId[0]);
	//	fluidFragShader->Validate();

	//	quad->RenderQuad();
	//}

	//void Render2DSmoke() 
	//{
	//	glViewport(0, 0, ScreenWidth, ScreenHeight);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//	glm::vec2 pos = glm::vec2(simWidth / 2, 75);

	//	addSplatSpot(pos, glm::vec3(0.05f, 0.05f, 0.05f), 0.5f, smokeDensity.get(), smokeDenTexId[0]);
	//	addSplatSpot(pos, glm::vec3(rd() * 20.0f + 10.0f, 0.0f, 0.0f), 3.0f, smokeTemperatureTexture.get(), smokeTempTexId[0]);
	//	addSplatSpot(pos, glm::vec3(2.0f * rd() - 1.0f, 0.0f, 0.0f), 5.0f, smokeVelocitiesTexture.get(), smokeVelTexId[0]);

	//	float vMax = maxReduce(smokeVelTexId[0], smokeVelocitiesTexture.get(), smokeFinalReduceTexture.get());
	//	if (vMax > 1e-10f) simDt = 5.0f / (vMax + simDt);

	//	mcAdvect(smokeVelocitiesTexture.get(), smokeVelocitiesTexture.get(), smokeVelTexId[0], smokeVelTexId);
	//	std::swap(smokeVelTexId[0], smokeVelTexId[3]);

	//	mcAdvect(smokeVelocitiesTexture.get(), smokeDensity.get(), smokeVelTexId[0], smokeDenTexId);
	//	std::swap(smokeDenTexId[0], smokeDenTexId[3]);

	//	mcAdvect(smokeVelocitiesTexture.get(), smokeTemperatureTexture.get(), smokeVelTexId[0], smokeTempTexId);
	//	std::swap(smokeTempTexId[0], smokeTempTexId[3]);

	//	applyBuoyantForce(smokeVelocitiesTexture.get(), smokeVelTexId[0], smokeTemperatureTexture.get(), smokeTempTexId[0], smokeDensity.get(), smokeDenTexId[0], 0.25f, 0.1f, 10.0f);

	//	RBMethod(smokeVelocitiesTexture.get(), smokeDivRBTexture.get(), smokePressureRBTexture.get(), smokeVelTexId[0], smokeVelTexId[1]);
	//	std::swap(smokeVelTexId[0], smokeVelTexId[1]);

	//	fluidFragShader->UseShader();
	//	smokeDensity->UseTextureArray(0);
	//	glUniform1i(glGetUniformLocation(fluidFragShader->GetShaderID(), "texIndex"), smokeDenTexId[0]);
	//	fluidFragShader->Validate();

	//	quad->RenderQuad();
	//}

	//void Render3DSmoke()
	//{
	//	glm::vec3 pos = glm::vec3(simDim3D / 2, simDim3D / 4, simDim3D / 2);

	//	//ToDo: #20 simulation manager class revert to this after fixing buyoancy
	//	/*addSplatSpot3D(pos, glm::vec3(0.05f, 0.05f, 0.05f), 0.5f, 1.0f, density3D.get()->at(0).get());
	//	addSplatSpot3D(pos, glm::vec3(rd() * 20.0f + 10.0f, 0.0f, 0.0f), 3.0f, 1.0f, temperature3D.get()->at(0).get());
	//	addSplatSpot3D(pos, glm::vec3(2.0f * rd() - 1.0f, 0.0f, 0.0f), 5.0f, 1.0f, veloxity3D.get()->at(0).get());*/

	//	addSplatSpot3D(pos, glm::vec3(0.5f, 0.5f, 0.5f), 0.5f, 1.0f, density3D.get()->at(0).get());
	//	//addSplatSpot3D(pos, glm::vec3(rd() * 20.0f + 10.0f, 0.0f, 0.0f), 3.0f, 1.0f, temperature3D.get()->at(0).get());
	//	addSplatSpot3D(pos, glm::vec3(2.0f * rd() - 1.0f,  1.0f, 0.0f), 5.0f, 1.0f, veloxity3D.get()->at(0).get());

	//	//ToDo
	//	//float vMax = maxReduce(smokeVelTexId[0], smokeVelocitiesTexture.get(), smokeFinalReduceTexture.get());
	//	//if (vMax > 1e-10f) simDt = 5.0f / (vMax + simDt);
	//	
	//	//ToDo	#20 simulation manager class calculate delta time from current max velocity field
	//	simDt3D = 0.05f;
	//	mcAdvect3D(veloxity3D.get()->at(0).get(), veloxity3D.get());
	//	veloxity3D.get()->at(0).swap(veloxity3D.get()->at(3));

	//	mcAdvect3D(veloxity3D.get()->at(0).get(), density3D.get());
	//	density3D.get()->at(0).swap(density3D.get()->at(3));

	//	//ToDo
	//	//mcAdvect3D(veloxity3D.get()->at(0).get(), temperature3D.get());
	//	//temperature3D.get()->at(0).swap(temperature3D.get()->at(3));

	//	//ToDo
	//	//applyBuoyantForce3D(veloxity3D.get()->at(0).get(), temperature3D.get()->at(0).get(), density3D.get()->at(0).get(), 0.25f, 0.1f, 10.0f);

	//	RBMethod3D(veloxity3D.get()->at(0).get(), veloxity3D.get()->at(1).get(), div3D.get(), pressure3D.get());
	//	veloxity3D.get()->at(0).swap(veloxity3D.get()->at(1));

	//	fluidFragShader3D->UseShader();

	//	auto model = glm::mat4(1.0f);
	//	model = glm::translate(model, glm::vec3(0.0f, 45.0f, 0.0f));
	//	model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
	//	glUniformMatrix4fv(fluidFragShader3D->GetModelLocation(), 1, GL_FALSE, glm::value_ptr(model));
	//	glUniformMatrix4fv(fluidFragShader3D->GetProjectionLocation(), 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
	//	glUniformMatrix4fv(fluidFragShader3D->GetViewLocation(), 1, GL_FALSE, glm::value_ptr(camera->CalculateViewMatrix()));
	//	glUniform3f(fluidFragShader3D->GetEyePositionLocation(), camera->getCameraPosition().x, camera->getCameraPosition().y, camera->getCameraPosition().z);

	//	density3D.get()->at(0)->UseTextureReadWrite(0, false, true);
	//	glUniform3f(glGetUniformLocation(fluidFragShader3D->GetShaderID(), "box_size"), simDim3D, simDim3D, simDim3D);
	//	fluidFragShader3D->Validate();

	//	ccw_cube->RenderCCWCube();
	//}

	void RenderToDefaultFB()
	{
		//ToDo: Clear after fluid demo #20 simulation manager class
		//if (drawFluidSim) 
		//{
			//Render2DFluidSim();
		//}
		//else if(drawSmokeSim)
		//{
			//Render2DSmoke();
		//}
		//else
		//{
			glViewport(0, 0, finalFBO->GetFBOWidth(), finalFBO->GetFBOHeight());

			finalFBO->BindFBO();
			//glViewport(0, 0, ScreenWidth, ScreenHeight);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			hdrShader->UseShader();

			glUniform1i(hdrShader->GetHDRLocation(), 1);
			glUniform1f(hdrShader->GetExposureLocation(), 1.0f);

			blur->AttachFBOToTextureUnit(1, GL_TEXTURE1, 0, 0);
			glUniform1i(hdrShader->GetBlurLocation(), 1);

			motionBlur->AttachFBOToTextureUnit(0, GL_TEXTURE2, 0, 0);
			hdrShader->SetTexture(2);

			hdrShader->Validate();

			quad->RenderQuad();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//}
	}

	~Impl()
	{
		if (AABBvolumeGridSSBO != 0)
		{
			glDeleteBuffers(1, &AABBvolumeGridSSBO);
			AABBvolumeGridSSBO = 0;
		}
		if (screenToViewSSBO != 0)
		{
			glDeleteBuffers(1, &screenToViewSSBO);
			screenToViewSSBO = 0;
		}
		if (lightSSBO != 0)
		{
			glDeleteBuffers(1, &lightSSBO);
			lightSSBO = 0;
		}
		if (lightIndexListSSBO != 0)
		{
			glDeleteBuffers(1, &lightIndexListSSBO);
			lightIndexListSSBO = 0;
		}
		if (lightGridSSBO != 0)
		{
			glDeleteBuffers(1, &lightGridSSBO);
			lightGridSSBO = 0;
		}
		if (visibleClusterSSBO != 0)
		{
			glDeleteBuffers(1, &visibleClusterSSBO);
			visibleClusterSSBO = 0;
		}
	}
};

RenderEngineMain::RenderEngineMain() : m_pImpl{ std::make_unique<Impl>() } { Pimpl()->Init(); };

void RenderEngineMain::Update()
{	
	Pimpl()->Update();
}

void RenderEngineMain::EndUpdate() 
{
	Pimpl()->EndUpdate();
}

GLFWwindow* RenderEngineMain::GetMainWindow()
{	
	return Pimpl()->mainWindow->GetWindow();
}

void RenderEngineMain::AddViewers()
{
	engineUI->AddSceneViewers(Pimpl()->ssaoBlur->GetFBOBuffer(0, 0), "EditorScene", Editor, [this](bool isSelected) { Pimpl()->isEditorViewSelected = isSelected; });
	engineUI->AddSceneViewers(Pimpl()->finalFBO->GetFBOBuffer(0, 0), "InGameScene", InGame, [this](bool isSelected) { Pimpl()->mainWindow->SetCursorActive(!isSelected); Pimpl()->isGameViewSelected = isSelected; });
}

bool RenderEngineMain:: IsEnd()
{
	return Pimpl()->mainWindow->getShouldClose();
}

RenderEngineMain::~RenderEngineMain() = default;