#include "pch.h"
#include "RenderEngineMain.h"

#include "Billboard_Shader.h"
#include "Particle_Shader.h"
#include "Compute_Shader.h"

#include "Mesh.h"
#include "Billboard_Mesh.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Particle.h"

#include "Scene_Fbo_Handler_Manager.h"
#include "Fbo_Handler.h"

#include "Model_Importer.h"

#include "ParticleSystem.h"
#include "Skybox.h"

#include "Debug.h"

#include "RenderingCommonValues.h"
#include "MathUtil.h"
#include "Window.h"

#include "PhysicsEngineMain.h"
#include "EngineUIMain.h"
#include "Shader_Object.h"

#include "Render_Object.h"
#include "Brdf_Render_Pass_Handler.h"
#include "Prefilter_Render_Pass_Handler.h"
#include "Environment_Map_Render_Pass_Handler.h"
#include "Irradiance_Convolution_Render_Pass_Handler.h"
#include "Directional_Shadow_Map_Render_Pass_Handler.h"
#include "Omni_Directional_Shadow_Map_Render_Pass_Handler.h"
#include "PreZ_Render_Pass_Handler.h"
#include "Ssao_Render_Pass_Handler.h"
#include "Ssao_Blur_Render_Pass_Handler.h"
#include "Scene_Render_Pass_Handler.h"
#include "Bloom_Render_Pass_Handler.h"
#include "Motion_Blur_Render_Pass_Handler.h"
#include "Exposure_Render_Pass_Handler.h"

#include <glm/gtx/euler_angles.hpp>

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

	std::shared_ptr <Shader_Object> environmentMapShader;
	std::shared_ptr <Shader_Object> irradianceConvolutionShader;
	std::shared_ptr <Shader_Object> prefilterShader;
	std::shared_ptr <Shader_Object> brdfShader;
	std::shared_ptr <Shader_Object> preZShader;
	std::shared_ptr <Shader_Object> animPreZShader;
	std::shared_ptr <Shader_Object> terrPreZShader;
	std::shared_ptr <Shader_Object> ssaoShader;
	std::shared_ptr <Shader_Object> ssaoBlurShader;
	std::shared_ptr <Shader_Object> omniShadowShader;
	std::shared_ptr <Shader_Object> dirShadowShader;
	std::shared_ptr <Shader_Object> animOmniShadowShader;
	std::shared_ptr <Shader_Object> animDirShadowShader;
	std::shared_ptr <Shader_Object> terrOmniShadowShader;
	std::shared_ptr <Shader_Object> terrDirShadowShader;
	std::shared_ptr <Shader_Object> unrigShader;
	std::shared_ptr <Shader_Object> rigShader;
	std::shared_ptr <Shader_Object> terrShader;
	std::shared_ptr <Shader_Object> bloomShader;
	std::shared_ptr <Shader_Object> motionBlurShader;
	std::shared_ptr <Shader_Object> exposureShader;

	std::shared_ptr <Fbo_Handler> environmentMap;
	std::shared_ptr <Fbo_Handler> irradianceMap;
	std::shared_ptr <Fbo_Handler> prefilterMap;
	std::shared_ptr <Fbo_Handler> brdfMap;
	std::shared_ptr <Fbo_Handler> depthMap;
	std::shared_ptr <Fbo_Handler> ssaoFbo;
	std::shared_ptr <Fbo_Handler> ssaoBlurFbo;
	std::shared_ptr <Fbo_Handler> sceneFbo;
	std::shared_ptr <Fbo_Handler> motionBlurFbo;
	std::shared_ptr <Fbo_Handler> bloomFbo;
	std::shared_ptr <Fbo_Handler> exposureFbo;

	std::unique_ptr < Shader_Object > billboardShader;
	std::unique_ptr < Particle_Shader> particleShader = std::make_unique<Particle_Shader>();

	//ToDo: To whomever it may concern. Probably me -_-
	//ToDo: After a decently extensive research and thought on my part I want the following to be done
	//ToDo: Add Memory Pooling.
	//ToDo: Each Object will have the data stored here while they only store weak_ptrs maybe. Need to test.
	//Pros: 
		// 1: Faster Pimpl Idiom implementation
		// 2: Better cache locality (both, temporal and spactial)
		// 3: Less page faults when virtual memory is being used
		// 4: Better memory coherency.
		// 5: Partial Data oriented design, so faster for loops, especially when searching for data.
		// 6: Contiguous data always wins.
	//Cons:
		// 1: Can have large memory footprint, especially when proper bucket size can't be found.
		// 2: Pimpl can still be slow due to one unnecessary indirection.
		// 3: May cause dangling pointer because RAII is not being used anymore with this design.
		// 4: If weak_ptrs are used as references to the data in the pool then locking them in can be slow to being atomic operation.
	//Possible solutions to the cons:
		//(1) Runtime memory bucket creation depending on stride of the data type?? 
		//(2) Can be alleviated if we store vector<Data> rather than vector<Data*> one less indirection for non-polymorphic classes.
		//(3) Users should never handle deleting stuff by themselves? Weak_ptr being used guarantees intent of the creator tho.
		//(4) Can be solved if we use seperate memory pools per each thread.

	//ToDo: Write everything with the above design in mind.

	std::shared_ptr<std::vector<std::shared_ptr<Texture>>> texturePool;
	std::shared_ptr<std::vector<std::shared_ptr<Mesh>>> meshPool;
	std::shared_ptr<std::vector<std::shared_ptr<glm::mat4>>> modelMatrixPool;
	std::shared_ptr<std::vector<std::shared_ptr<glm::mat4>>> prevModelMatrixPool;
	std::shared_ptr<std::vector<std::vector<std::shared_ptr<Render_Object>>>> quadRO;
	std::shared_ptr<std::vector<std::vector<std::shared_ptr<Render_Object>>>> cwCubeRO;
	std::shared_ptr<std::vector<std::vector<std::shared_ptr<Render_Object>>>> sceneObjRO;

	//ToDo: vector of all virtual objects should be in a different class
	static std::shared_ptr<std::vector<std::shared_ptr<Transform>>> transformPool;
	std::shared_ptr<std::vector<std::shared_ptr<VObject>>> vObjectPool;

	void AddToMeshPool(std::shared_ptr<Mesh>&& mesh)
	{
		meshPool->push_back(std::move(mesh));
	}
	void AddToModelMatrixPool(std::shared_ptr<glm::mat4>&& mat)
	{
		modelMatrixPool->push_back(std::move(mat));
	}
	void AddToPrevModelMatrixPool(std::shared_ptr<glm::mat4>&& mat)
	{
		prevModelMatrixPool->push_back(std::move(mat));
	}

	std::unique_ptr<std::map<TexType, std::vector<std::weak_ptr<Texture>>>> CreateTextureMap(std::vector<TexMapData>&& texMapData)
	{
		auto textureMap = std::make_unique<std::map<TexType, std::vector<std::weak_ptr<Texture>>>>();
		
		for(auto i = 0; i< texMapData.size(); ++i)
		{
			TexMapData& dat = texMapData[i];
			auto texture = std::make_shared<Texture>(dat.path, dat.type == TexType::Albedo);
			auto wTexPtr = std::weak_ptr<Texture>(texture);
			texturePool->push_back(texture);
			//ToDo check for alpha channel/ currently incorrect use of find
			if (dat.path.find(".png" || ".gif" || ".tiff" || ".tga" || ".jp2" || ".jpx") != std::string::npos)
			{
				texture->LoadTextureWithAlpha();
			}
			else
			{
				texture->LoadTextureNoAlpha();
			}
			if (textureMap->contains(dat.type))
			{
				textureMap->at(dat.type).push_back(texture);
			}
			else
			{
				auto vec = std::vector<std::weak_ptr<Texture>>();
				vec.push_back(wTexPtr);
				textureMap->emplace(dat.type, vec);
			}
		}

		return textureMap;
	}

	std::vector< std::shared_ptr < Billboard_Mesh>> billboardList;
	std::vector< std::shared_ptr < ParticleSystem>> particleList;

	std::unique_ptr<std::vector<std::weak_ptr<Mesh>>> quad;
	std::unique_ptr<std::vector<std::weak_ptr<Mesh>>> cwCube;
	std::shared_ptr < Mesh> ccw_cube;

	std::shared_ptr < Camera> camera;

	std::shared_ptr < DirectionalLight> mainLight;
	std::shared_ptr < PointLight> pointLights[MAX_POINT_LIGHTS_WITH_SHADOW];
	std::shared_ptr < SpotLight> spotLights[MAX_SPOT_LIGHTS];
	std::shared_ptr <std::vector<std::shared_ptr<PointLight>>> omniDirLights;

	unsigned int pointLightCount = 0;
	unsigned int spotLightCount = 0;

	std::unique_ptr < Skybox> skybox;

	std::shared_ptr < Texture> environmentTexture;
	std::unique_ptr < Texture> skyboxTexture;

	std::shared_ptr < Texture> terrainTextureDisp;
	std::shared_ptr < Texture> terrainTextureBlend;
	std::shared_ptr < Texture> terrainTexture;
	std::shared_ptr < Texture> terrainTextureMetal;
	std::shared_ptr < Texture> terrainTextureRough;
	std::shared_ptr < Texture> terrainTextureNorm;
	std::shared_ptr < Texture> terrainTexturePara;
	
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
 
	std::shared_ptr < Texture> SSAONoiseTexture = std::make_shared<Texture>();

	std::unique_ptr < Texture> plainTexture;
	std::unique_ptr < Texture> grassTexture;

	std::shared_ptr <Render_Object> pyramid1;
	std::shared_ptr <Render_Object> pyramid2;
	std::shared_ptr <Render_Object> rectangle1;
	std::shared_ptr <Render_Object> rectangle2;

	std::shared_ptr <Render_Object> sniper;
	std::shared_ptr <Render_Object> gun;
	std::shared_ptr <Render_Object> anymodel;
	std::shared_ptr <Render_Object> cube;
	std::shared_ptr <Render_Object> sphere;
	std::shared_ptr <Render_Object> bulbWhite;
	std::shared_ptr <Render_Object> bulbRed;

	std::shared_ptr<Brdf_Render_Pass_Handler> brdfRPHandler;
	std::shared_ptr<Prefilter_Render_Pass_Handler> prefilterRPHandler;
	std::shared_ptr<Environment_Map_Render_Pass_Handler> envMapRPHandler;
	std::shared_ptr<Irradiance_Convolution_Render_Pass_Handler> irrConvRPHandler;
	std::shared_ptr<Directional_Shadow_Map_Render_Pass_Handler> dirShadowRPHandler;
	std::shared_ptr<Omni_Directional_Shadow_Map_Render_Pass_Handler> omniShadowRPHandler;
	std::shared_ptr<PreZ_Render_Pass_Handler> preZRPHandler;
	std::shared_ptr<Ssao_Render_Pass_Handler> ssaoRPHandler;
	std::shared_ptr<Ssao_Blur_Render_Pass_Handler> ssaoBlurRPHandler;
	std::shared_ptr<Scene_Render_Pass_Handler> sceneRPHandler;
	std::shared_ptr<Bloom_Render_Pass_Handler> bloomRPHandler;
	std::shared_ptr<Motion_Blur_Render_Pass_Handler> motionBlurRPHandler;
	std::shared_ptr<Exposure_Render_Pass_Handler> exposureRPHandler;

	GLfloat aircraftAngle = 0.0f;

	glm::mat4 vView[NUM_CASCADES] = { glm::mat4() };

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
		//CreateBillboard();
		//CreateParticles();

		texturePool = std::make_shared<std::vector<std::shared_ptr<Texture>>>();
		meshPool = std::make_shared<std::vector<std::shared_ptr<Mesh>>>();
		modelMatrixPool = std::make_shared<std::vector<std::shared_ptr<glm::mat4>>>();
		prevModelMatrixPool = std::make_shared<std::vector<std::shared_ptr<glm::mat4>>>();
		sceneObjRO = std::make_shared<std::vector<std::vector<std::shared_ptr<Render_Object>>>>();

		texturePool->reserve(100);
		meshPool->reserve(100);
		modelMatrixPool->reserve(100);
		prevModelMatrixPool->reserve(100);
		sceneObjRO->reserve(100);

		CreateTerrain();
		CreateObject();
		CreateShaders();

		camera = std::make_shared<Camera>(glm::vec3(-terrainScaleFactor, 0.0f, -terrainScaleFactor + 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 50.0f, 0.2f);

		environmentTexture = std::make_shared<Texture>("Textures/HDR/GCanyon_C_YumaPoint_3k.hdr");
		environmentTexture->LoadTextureHDR();

		plainTexture = std::make_unique <Texture>("Textures/plain.png", true);
		plainTexture->LoadTextureWithAlpha();

		grassTexture = std::make_unique <Texture>("Textures/grass.png", true);
		grassTexture->LoadTextureWithAlpha();

		//pyramid1 = std::make_unique<Static_Object>();
		//pyramid1->SetUpNativeModelData(meshList[0], "Textures/rustediron2.png", "Textures/Metallic/rustediron2.png",
		//	"Textures/Roughness/rustediron2.png", "Textures/Normal/rustediron2.png",
		//	"Textures/Parallax/rustediron2.png", "Textures/Glow/rock.jpg");
		//pyramid2 = std::make_unique<Static_Object>();
		//pyramid2->SetUpNativeModelData(meshList[1], "Textures/small_metal_debris.jpg", "Textures/Metallic/small_metal_debris.jpg",
		//	"Textures/Roughness/small_metal_debris.jpg", "Textures/Normal/small_metal_debris.jpg",
		//	"Textures/Parallax/small_metal_debris.jpg", "Textures/Glow/small_metal_debris.jpg");
		//rectangle1 = std::make_unique<Static_Object>();
		//rectangle1->SetUpNativeModelData(meshList[2], "Textures/brick.jpg", "Textures/Metallic/brick.jpg",
		//	"Textures/Roughness/brick.jpg", "Textures/Normal/brick.jpg",
		//	"Textures/Parallax/brick.jpg", "Textures/Glow/brick.jpg");
		//rectangle2 = std::make_unique<Static_Object>();
		//rectangle2->SetUpNativeModelData(meshList[3], "Textures/brick_floor.png", "Textures/Metallic/brick_floor.png",
		//	"Textures/Roughness/brick_floor.png", "Textures/Normal/brick_floor.png",
		//	"Textures/Parallax/brick_floor.png", "Textures/Glow/brick_floor.png");


		terrainTextureDisp = std::make_shared <Texture>("Textures/Displacement/terrain.jpg");
		terrainTextureDisp->LoadTextureNoAlpha();
		terrainTextureBlend = std::make_shared <Texture>("Textures/Blend/terrain.jpg");
		terrainTextureBlend->LoadTextureNoAlpha();
		terrainTexture = std::make_shared <Texture>("Textures/terrain.jpg", true);
		terrainTexture->LoadTextureArray(glm::vec2(1024, 1024), NUM_TERRAIN_LAYERS);
		terrainTextureMetal = std::make_shared <Texture>("Textures/Metallic/terrain.jpg");
		terrainTextureMetal->LoadTextureArray(glm::vec2(256, 256), NUM_TERRAIN_LAYERS);
		terrainTextureRough = std::make_shared <Texture>("Textures/Roughness/terrain.jpg");
		terrainTextureRough->LoadTextureArray(glm::vec2(256, 256), NUM_TERRAIN_LAYERS);
		terrainTextureNorm = std::make_shared <Texture>("Textures/Normal/terrain.jpg");
		terrainTextureNorm->LoadTextureArray(glm::vec2(256, 256), NUM_TERRAIN_LAYERS);
		terrainTexturePara = std::make_shared <Texture>("Textures/Parallax/terrain.jpg");
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

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
		
		//auto unriggedSceneMeshes = std::vector<std::shared_ptr<Render_Object>>();

		//auto bulbModel = Static_Model();
		//bulbModel.LoadModel("Models/Free_Antique_Bulb.obj");
		//bulbWhite = std::make_shared<Render_Object>(std::move(bulbModel.MeshList), bulbModel.TextureMap);
		//unriggedSceneMeshes.push_back(bulbWhite);
		//
		//auto bulbRedModel = Static_Model();
		//bulbRedModel.LoadModel("Models/Free_Antique_Bulb.obj");
		//bulbRed = std::make_shared<Render_Object>(std::move(bulbRedModel.MeshList), bulbRedModel.TextureMap);
		//unriggedSceneMeshes.push_back(bulbRed);

		//auto sphereModel = Static_Model();
		//sphereModel.LoadModel("Models/sphere.obj");
		//sphere = std::make_shared<Render_Object>(std::move(sphereModel.MeshList), sphereModel.TextureMap);
		//unriggedSceneMeshes.push_back(sphere);
		////physicsEngine->AddSphere(1.0f, -terrainScaleFactor, 80.0f, 5.5f - terrainScaleFactor, 1.0f, sphere->());
		////ToDo: use heightmap to add terrain collider
		//physicsEngine->AddStaticPlane(0.0f, 27.0f, 0.0f, 0.0f, glm::vec3(0.0f, 0.9f, 0.1f), nullptr);

		//auto cubeModel = Static_Model();
		//cubeModel.LoadModel("Models/cube.obj");
		//cube = std::make_shared<Render_Object>(std::move(cubeModel.MeshList), cubeModel.TextureMap);
		//unriggedSceneMeshes.push_back(cube);

		//auto sniperModel = Static_Model();
		//sniperModel.LoadModel("Models/Sniper_rifle_KSR-29.fbx");
		//sniper = std::make_shared<Render_Object>(std::move(sniperModel.MeshList), sniperModel.TextureMap);
		//unriggedSceneMeshes.push_back(sniper);

		//auto gunModel = Static_Model();
		//gunModel.LoadModel("Models/Cerberus_LP.fbx");
		//gun = std::make_shared<Render_Object>(std::move(gunModel.MeshList), gunModel.TextureMap);
		//unriggedSceneMeshes.push_back(gun);

		//auto anyModel = Static_Model();
		//anyModel.LoadModel("Models/Intergalactic_Spaceship-(Wavefront).obj");
		//anymodel = std::make_shared<Render_Object>(std::move(anyModel.MeshList), anyModel.TextureMap);
		//unriggedSceneMeshes.push_back(anymodel);

		//sceneObjRO->push_back(unriggedSceneMeshes);

		//anim->LoadModel("Models/boblampclean.md5mesh");
		//anim2->LoadModel("Models/model.dae");

		m_SceneFboHandlerMgr = std::make_shared<Scene_Fbo_Handler_Manager>("InGame");

		environmentMap = m_SceneFboHandlerMgr->FindFboHandler("Environment_Map_Pass");
		irradianceMap = m_SceneFboHandlerMgr->FindFboHandler("Irradiance_Map_Pass");
		prefilterMap = m_SceneFboHandlerMgr->FindFboHandler("Pre_Filter_Pass");
		brdfMap = m_SceneFboHandlerMgr->FindFboHandler("Brdf_Pass");
		depthMap = m_SceneFboHandlerMgr->FindFboHandler("Depth_Pass");
		ssaoFbo = m_SceneFboHandlerMgr->FindFboHandler("Ssao_Pass");
		ssaoBlurFbo = m_SceneFboHandlerMgr->FindFboHandler("Ssao_Blur_Pass");
		sceneFbo = m_SceneFboHandlerMgr->FindFboHandler("Shading_Pass");
		motionBlurFbo = m_SceneFboHandlerMgr->FindFboHandler("Motion_Blur_Pass");
		bloomFbo = m_SceneFboHandlerMgr->FindFboHandler("Bloom_Pass");
		exposureFbo = m_SceneFboHandlerMgr->FindFboHandler("Final_Output_Pass");

		quad = std::make_unique <std::vector<std::weak_ptr<Mesh>>>();

		std::vector<GLuint> quadIndices = {
			0, 1, 2,
			2, 1, 3
		};

		std::vector<std::vector<GLfloat>> quadVertices =
		{
			std::vector<GLfloat>{ -1.0f, -1.0f, 0.0f,		 0.0f, 0.0f },
			std::vector<GLfloat>{  1.0f, -1.0f, 0.0f,		 1.0f, 0.0f },
			std::vector<GLfloat>{ -1.0f,  1.0f, 0.0f,		 0.0f, 1.0f },
			std::vector<GLfloat>{  1.0f,  1.0f, 0.0f,		 1.0f, 1.0f },
		};

		auto qMesh = std::make_shared<Mesh>(0, std::move(quadVertices), std::move(quadIndices), std::move(MeshGenParams()));
		quad->push_back(qMesh);
		AddToMeshPool(std::move(qMesh));
		quadRO = std::make_shared<std::vector<std::vector<std::shared_ptr<Render_Object>>>>();
		auto qro = std::make_shared<Render_Object>(std::move(quad));
		quadRO->push_back(std::vector<std::shared_ptr<Render_Object>>{qro});

		cwCube = std::make_unique<std::vector<std::weak_ptr<Mesh>>>();
		std::vector<GLuint> cwCubeIndices = {
			//front
			0,1,2,
			2,1,3,
			//right
			2,3,5,
			5,3,7,
			//back
			5,7,4,
			4,7,6,
			//left
			4,6,0,
			0,6,1,
			//top
			4,0,5,
			5,0,2,
			//bottom
			1,6,3,
			3,6,7
		};

		std::vector<std::vector<GLfloat>>  cwCubeVertices = {
			std::vector<GLfloat>{ -1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f },
			std::vector<GLfloat>{ -1.0f, -1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f },
			std::vector<GLfloat>{  1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f },
			std::vector<GLfloat>{  1.0f, -1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f },

			std::vector<GLfloat>{ -1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f },
			std::vector<GLfloat>{  1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f },
			std::vector<GLfloat>{ -1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f },
			std::vector<GLfloat>{  1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f }

		};

		auto cwMesh = std::make_shared<Mesh>(0, std::move(cwCubeVertices), std::move(cwCubeIndices), std::move(MeshGenParams()));
		cwCube->push_back(cwMesh);
		AddToMeshPool(std::move(cwMesh));
		cwCubeRO = std::make_shared<std::vector<std::vector<std::shared_ptr<Render_Object>>>>();
		auto cro = std::make_shared<Render_Object>(std::move(cwCube));
		cwCubeRO->push_back(std::vector<std::shared_ptr<Render_Object>>{cro});

		//CW=======================================================================
/*
		// Back face
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-left
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // bottom-right
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
		// Front face
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, // top-left
		// Left face
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-right
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-right
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-right
		// Right face
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-left
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-left
		// Bottom face
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-left
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, // top-left
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-right
		// Top face
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f  // top-left
*/
		std::vector<GLuint> ccwCubeIndices = {
			//back face (CCW)
			3, 1, 0,
			0, 2, 3,
			//front face (CCW)
			6, 7, 5,
			5, 4, 6,
			//left face (CCW)
			1, 6, 4,
			4, 0, 1,
			//right face (CCW)
			7, 3, 2,
			2, 5, 7,
			//bottom face (CCW)
			1, 3, 7,
			7, 6, 1,
			// top face (CCW)
			4, 5, 2,
			2, 0, 4
		};

		std::vector<std::vector<GLfloat>> ccwCubeVertices = {
			std::vector<GLfloat>{	-0.5f, 0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f },	//0 -+-
			std::vector<GLfloat>{	-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f },	//1 ---
			std::vector<GLfloat>{	0.5f, 0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f },	//2 ++-
			std::vector<GLfloat>{	0.5f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f },	//3 +--

			std::vector<GLfloat>{	-0.5f, 0.5f, 0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f },	//4 -++
			std::vector<GLfloat>{	0.5f, 0.5f, 0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f },	//5	+++
			std::vector<GLfloat>{	-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f },	//6 --+
			std::vector<GLfloat>{	0.5f, -0.5f, 0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f }	//7 +-+

		};

		//CCW==============================================================================================
/*
		// back face (CCW winding)
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // bottom-right
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
		// front face (CCW winding)
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, // top-left
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		// left face (CCW)
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
		-0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
		-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
		-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
		// right face (CCW)
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // bottom-right
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, // top-left
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		// bottom face (CCW)
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // bottom-right
		0.5f, -0.5f, 0.5f, 1.0f, 1.0f, // top-right
		0.5f, -0.5f, 0.5f, 1.0f, 1.0f, // top-right
		-0.5f, -0.5f, 0.5f, 0.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
		// top face (CCW)
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, // bottom-left

\*/
		ccw_cube = std::make_shared <Mesh>(0, std::move(ccwCubeVertices), std::move(ccwCubeIndices), std::move(MeshGenParams()));

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

		omniDirLights = std::make_shared<std::vector<std::shared_ptr<PointLight>>>();
		for (auto i = 0; i < pointLightCount; ++i){ omniDirLights->push_back(pointLights[i]); }
		for (auto i = 0; i < spotLightCount; ++i) { omniDirLights->push_back(spotLights[i]); }

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

		auto camParam = CamParam{ camera->getCameraPosition(), camera->GetProjectionMatrix(), camera->CalculateViewMatrix(), camera->GetPreviousProjectionMatrix() };
		CalcDirLightShadowCascades();
		InitSSBOs();
		CreateClusters();

		auto envMapShaders = std::vector<std::shared_ptr<Shader_Object>>{ environmentMapShader };
		envMapRPHandler = std::make_shared<Environment_Map_Render_Pass_Handler>(environmentMap, envMapShaders);
		auto envMapTexData = std::map<TexType, std::vector<std::weak_ptr<Texture>>>();
		envMapTexData.emplace(Default, std::vector<std::weak_ptr<Texture>>{environmentTexture});
		cwCubeRO->at(0)[0]->SetTextures(std::move(envMapTexData));
		envMapRPHandler->Update(*cwCubeRO);
		cwCubeRO->at(0)[0]->ResetTextures();

		auto irrConvShaders = std::vector<std::shared_ptr<Shader_Object>>{ irradianceConvolutionShader };
		auto inputs = std::make_shared<std::vector<std::shared_ptr<std::any>>>();
		inputs->push_back(std::make_shared<std::any>(std::make_any<std::shared_ptr<Fbo_Handler>>(environmentMap)));
		irrConvRPHandler = std::make_shared<Irradiance_Convolution_Render_Pass_Handler>(irradianceMap, irrConvShaders, inputs);
		irrConvRPHandler->Update(*cwCubeRO);

		auto prefilterShaders = std::vector<std::shared_ptr<Shader_Object>>{ prefilterShader };
		prefilterRPHandler = std::make_shared<Prefilter_Render_Pass_Handler>(prefilterMap, prefilterShaders, inputs);
		prefilterRPHandler->Update(*cwCubeRO);

		auto brdfShaders = std::vector<std::shared_ptr<Shader_Object>>{ brdfShader };
		brdfRPHandler = std::make_shared<Brdf_Render_Pass_Handler>(brdfMap, brdfShaders);
		brdfRPHandler->Update(*quadRO);

		auto dirShadowShaders = std::vector<std::shared_ptr<Shader_Object>>{ dirShadowShader, animDirShadowShader, terrDirShadowShader };
		dirShadowRPHandler = std::make_shared<Directional_Shadow_Map_Render_Pass_Handler>(mainLight->GetShadowMap(), dirShadowShaders);

		auto ommiShadowShaders = std::vector<std::shared_ptr<Shader_Object>>{ omniShadowShader, animOmniShadowShader };
		omniShadowRPHandler = std::make_shared<Omni_Directional_Shadow_Map_Render_Pass_Handler>(pointLights[0]->GetShadowMap(), ommiShadowShaders);

		auto prezShaders = std::vector<std::shared_ptr<Shader_Object>>{ preZShader, animPreZShader, terrPreZShader};
		preZRPHandler = std::make_shared<PreZ_Render_Pass_Handler>(depthMap, prezShaders);

		auto ssaoShaders = std::vector<std::shared_ptr<Shader_Object>>{ ssaoShader };
		inputs = std::make_shared<std::vector<std::shared_ptr<std::any>>>();
		inputs->push_back(std::make_shared<std::any>(std::make_any<std::shared_ptr<Fbo_Handler>>(depthMap)));
		ssaoRPHandler = std::make_shared<Ssao_Render_Pass_Handler>(ssaoFbo, ssaoShaders, inputs);
		InitSSAO();

		auto ssaoBlurShaders = std::vector<std::shared_ptr<Shader_Object>>{ ssaoBlurShader };
		inputs = std::make_shared<std::vector<std::shared_ptr<std::any>>>();
		inputs->push_back(std::make_shared<std::any>(std::make_any<std::shared_ptr<Fbo_Handler>>(ssaoFbo)));
		ssaoBlurRPHandler = std::make_shared<Ssao_Blur_Render_Pass_Handler>(ssaoBlurFbo, ssaoBlurShaders, inputs);

		auto sceneShaders = std::vector<std::shared_ptr<Shader_Object>>{ unrigShader, rigShader, terrShader };
		inputs = std::make_shared<std::vector<std::shared_ptr<std::any>>>();
		inputs->push_back(std::make_shared<std::any>(std::make_any<std::shared_ptr<Fbo_Handler>>(mainLight->GetShadowMap())));
		inputs->push_back(std::make_shared<std::any>(std::make_any<std::shared_ptr<Fbo_Handler>>(pointLights[0]->GetShadowMap())));
		inputs->push_back(std::make_shared<std::any>(std::make_any<std::shared_ptr<Fbo_Handler>>(irradianceMap)));
		inputs->push_back(std::make_shared<std::any>(std::make_any<std::shared_ptr<Fbo_Handler>>(prefilterMap)));
		inputs->push_back(std::make_shared<std::any>(std::make_any<std::shared_ptr<Fbo_Handler>>(brdfMap)));
		inputs->push_back(std::make_shared<std::any>(std::make_any<std::shared_ptr<Fbo_Handler>>(ssaoBlurFbo)));
		inputs->push_back(std::make_shared<std::any>(std::make_any<std::shared_ptr<Fbo_Handler>>(depthMap)));
		sceneRPHandler = std::make_shared<Scene_Render_Pass_Handler>(sceneFbo, sceneShaders, inputs);

		auto bloomShaders = std::vector<std::shared_ptr<Shader_Object>>{ bloomShader };
		inputs = std::make_shared<std::vector<std::shared_ptr<std::any>>>();
		inputs->push_back(std::make_shared<std::any>(std::make_any<std::shared_ptr<Fbo_Handler>>(sceneFbo)));
		bloomRPHandler = std::make_shared<Bloom_Render_Pass_Handler>(bloomFbo, bloomShaders, inputs);

		auto motionBlurShaders = std::vector<std::shared_ptr<Shader_Object>>{ motionBlurShader };
		inputs = std::make_shared<std::vector<std::shared_ptr<std::any>>>();
		inputs->push_back(std::make_shared<std::any>(std::make_any<std::shared_ptr<Fbo_Handler>>(sceneFbo)));
		motionBlurRPHandler = std::make_shared<Motion_Blur_Render_Pass_Handler>(motionBlurFbo, motionBlurShaders, inputs);

		auto exosureShaders = std::vector<std::shared_ptr<Shader_Object>>{ exposureShader };
		inputs = std::make_shared<std::vector<std::shared_ptr<std::any>>>();
		inputs->push_back(std::make_shared<std::any>(std::make_any<std::shared_ptr<Fbo_Handler>>(bloomFbo)));
		inputs->push_back(std::make_shared<std::any>(std::make_any<std::shared_ptr<Fbo_Handler>>(motionBlurFbo)));
		exposureRPHandler = std::make_shared<Exposure_Render_Pass_Handler>(exposureFbo, exosureShaders, inputs);

		//sceneObjRO->push_back(unriggedSceneMeshes);
		
		//auto riggedSceneMeshes = std::vector <std::shared_ptr<Render_Object>>();
		//riggedSceneMeshes.push_back(std::make_shared<Render_Object>(std::make_shared<std::vector<std::shared_ptr<Mesh>>>(anim->MeshList)));
		//riggedSceneMeshes.push_back(std::make_shared<Render_Object>(std::make_shared<std::vector<std::shared_ptr<Mesh>>>(anim2->MeshList)));

		//sceneObjRO->push_back(riggedSceneMeshes);

		//auto terrainSceneMeshes = std::vector<std::shared_ptr<Render_Object>>();

		//textureMap = std::make_shared<std::map<TexType, std::vector<std::shared_ptr<Texture>>>>();
		//textureMap->emplace(TexType::Albedo, std::vector<std::shared_ptr<Texture>>{terrainTexture});
		//textureMap->emplace(TexType::Metallic, std::vector<std::shared_ptr<Texture>>{terrainTextureMetal});
		//textureMap->emplace(TexType::Roughness, std::vector<std::shared_ptr<Texture>>{terrainTextureRough});
		//textureMap->emplace(TexType::Normal, std::vector<std::shared_ptr<Texture>>{terrainTextureNorm});
		//textureMap->emplace(TexType::Parallax, std::vector<std::shared_ptr<Texture>>{terrainTexturePara});
		//textureMap->emplace(TexType::Displacement, std::vector<std::shared_ptr<Texture>>{terrainTextureDisp});
		//terrainSceneMeshes.push_back(std::make_shared<Render_Object>(std::make_shared<std::vector<std::shared_ptr<Mesh>>>(std::vector<std::shared_ptr<Mesh>>{terrainList->at(0)}), textureMap, std::make_shared<glm::mat4>(1.0f), std::make_shared<glm::mat4>(1.0f)));

		//sceneObjRO->push_back(terrainSceneMeshes);
	}

	void InitSSAO() 
	{
		//SSAO initialization
		ssaoRPHandler->Init();
		std::vector<glm::vec3> ssaoNoiseData{ 16, glm::vec3(0.0f, 0.0f, 0.0f) };
		for (unsigned int i = 0; i < 16; i++)
		{
			glm::vec3 noise(
				2.0f * (float)rand() / RAND_MAX - 1.0f,
				2.0f * (float)rand() / RAND_MAX - 1.0f,
				0.0f);
			ssaoNoiseData[i] = noise;
		}
		SSAONoiseTexture->LoadNativeTexture(ssaoNoiseData);
	}

	void CalcDirLightShadowCascades() 
	{
		//ToDo;
		/*terrainShader->UseShader();
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
		}*/
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

			UpdateObjectTransforms();
			auto camParam = CamParam{ camera->getCameraPosition(), camera->GetProjectionMatrix(), camera->CalculateViewMatrix(), camera->GetPreviousProjectionMatrix(), framesPerSec };
			DirectionalShadowMapPass(mainLight.get(), camParam);

			OmniShadowMapPass(*omniDirLights);

			PreZPass(camParam);
			CullLight();
			SSAOPass(camParam);
			SSAOBlurPass(camParam);
			RenderPass(camParam, mainLight.get(), pointLights[0].get(), spotLights[0].get());
			//skybox->DrawHDRSkybox();
			Bloom();
			MotionBlurPass(camParam);

			auto lowerLight = camParam.Position;
			lowerLight.y -= 0.1f;
			spotLights[0]->SetFlash(lowerLight, camera->getCameraDirection());
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
				lightList.get()->at(i).position = glm::vec4(light->position, 1.0f);
				lightList.get()->at(i).color = glm::vec4(light->color, 1.0f);
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
		std::vector<GLuint> terrainIndices = {
			0, 2, 1,
			1, 2, 3
		};

		std::vector<std::vector<GLfloat>> terrainVertices = {
			std::vector<GLfloat>{	-terrainScaleFactor1, 0.0f,-terrainScaleFactor1,		0.0f, 0.0f,								        0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f },
			std::vector<GLfloat>{	terrainScaleFactor1, 0.0f,-terrainScaleFactor1,			terrainScaleFactor1, 0.0f,						0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f },
			std::vector<GLfloat>{	-terrainScaleFactor1, 0.0f, terrainScaleFactor1,		0.0f, terrainScaleFactor1,						0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f },
			std::vector<GLfloat>{	terrainScaleFactor1, 0.0f,terrainScaleFactor1,			terrainScaleFactor1, terrainScaleFactor1,	    0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f }
		};

		MathUtil::CalcAverageNormals(terrainIndices, terrainVertices, 5);
		MathUtil::CalcAverageTangents(terrainIndices, terrainVertices, 8);

		//Debug::DebugPrintReferenceTBN("ReferenceTBN", terrainVertices, 11, glm::vec3(0.0f, 1.0f, 0.0f));
		//Debug::DebugPrintTBN("TerrainTBN", terrainVertices, 5, 8);

		//auto obj = std::make_shared<Mesh>(0, std::move(terrainVertices), std::move(terrainIndices), std::move(MeshGenParams{ true, true, true }));
		//terrainList->push_back(obj);
	}

	std::shared_ptr<Mesh> CreatePrism()
	{
		std::vector<GLuint> indices = {
					1,3,0,
					2,3,1,
					0,3,2,
					2,1,0
		};

		std::vector<std::vector<GLfloat>> vertices = {
			//x      y      z		u		v		nx    ny	nz      tx	  ty    tz
			std::vector<GLfloat>{	-1.0f, -1.0f, -0.6f,	 0.0f, 0.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f	},	//bottom left
			std::vector<GLfloat>{	0.0f, -1.0f, 1.0f,		 1.0f, 0.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f	},   // z axis point
			std::vector<GLfloat>{	1.0f, -1.0f, -0.6f,		 0.0f, 1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f	},   //bottom right
			std::vector<GLfloat>{	0.0f, 1.0f, 0.0f,		 1.0f, 1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f	}    //top
		};

		MathUtil::CalcAverageNormals(indices, vertices, 5);
		MathUtil::CalcAverageTangents(indices, vertices, 8);
	
		return std::make_shared<Mesh>(0, std::move(vertices), std::move(indices), std::move(MeshGenParams{true, true}));
	}

	std::shared_ptr<Mesh> CreatePlane()
	{
		std::vector<GLuint> indices = {
			0, 2, 1,
			1, 2, 3
		};

		std::vector<std::vector<GLfloat>> vertices = {
			std::vector<GLfloat>{	-15.f, 0.0f, -15.0f,	0.0f, 0.0f,	    0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f	},
			std::vector<GLfloat>{	15.0f, 0.0f, -15.0f,	1.0f, 0.0f,	    0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f	},
			std::vector<GLfloat>{	-15.0f, 0.0f, 15.0f,	0.0f, 1.0f,	    0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f	},
			std::vector<GLfloat>{	15.0f, 0.0f, 15.0f,		1.0f, 1.0f,	    0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f	}
		};

		MathUtil::CalcAverageNormals(indices, vertices, 5);
		MathUtil::CalcAverageTangents(indices, vertices, 8);

		return std::make_shared<Mesh>(0, std::move(vertices), std::move(indices), std::move(MeshGenParams{ true, true }));
	}

	void CreateObject() 
	{	
		auto mesh0 = CreatePrism();
		auto texMapDatas = std::vector<TexMapData>();
		texMapDatas.push_back(TexMapData{ TexType::Albedo,		"Textures/rustediron2.png" });
		texMapDatas.push_back(TexMapData{ TexType::Metallic,	"Textures/Metallic/rustediron2.png" });
		texMapDatas.push_back(TexMapData{TexType::Roughness,	"Textures/Roughness/rustediron2.png"});
		texMapDatas.push_back(TexMapData{ TexType::Normal,		"Textures/Normal/rustediron2.png" });
		texMapDatas.push_back(TexMapData{ TexType::Parallax,	"Textures/Parallax/rustediron2.png" });
		texMapDatas.push_back(TexMapData{ TexType::Glow,		"Textures/Glow/rock.jpg" });
		auto modelMatrix = std::make_shared<glm::mat4>(1.0f);
		auto prevModelMatrix = std::make_shared<glm::mat4>(1.0f);

		auto texMap = CreateTextureMap(std::move(texMapDatas));

		std::unique_ptr<std::vector<std::weak_ptr<Mesh>>> mesh = std::make_unique<std::vector<std::weak_ptr<Mesh>>>();
		mesh->push_back(mesh0);
		AddToMeshPool(std::move(mesh0));
		
		auto mro = std::make_shared<Render_Object>(std::move(mesh), std::move(texMap), modelMatrix, prevModelMatrix);

		AddToModelMatrixPool(std::move(modelMatrix));
		AddToPrevModelMatrixPool(std::move(prevModelMatrix));

		sceneObjRO->push_back(std::vector<std::shared_ptr<Render_Object>>{mro});


		//auto rob = RenderObjectData{ std::make_shared<std::vector<Mesh>>(CreatePrism()), std::make_shared<std::map<TexType, std::vector<Texture>>>(), std::make_shared<glm::mat4>(1.0f), std::make_shared<glm::mat4>(1.0f) };
		//rob.CreateTextureMap(TexType::Albedo, "Textures/small_metal_debris.jpg");
		//rob.CreateTextureMap(TexType::Metallic, "Textures/Metallic/small_metal_debris.jpg");
		//rob.CreateTextureMap(TexType::Roughness, "Textures/Roughness/small_metal_debris.jpg");
		//rob.CreateTextureMap(TexType::Normal, "Textures/Normal/small_metal_debris.png");
		//rob.CreateTextureMap(TexType::Parallax, "Textures/Parallax/small_metal_debris.jpg");
		//rob.CreateTextureMap(TexType::Glow, "Textures/Glow/small_metal_debris.jpg");
		//renderObjDatas->push_back(std::make_shared<RenderObjectData>(rob));


		//rob = RenderObjectData{ std::make_shared<std::vector<Mesh>>(CreatePlane()), std::make_shared<std::map<TexType, std::vector<Texture>>>(), std::make_shared<glm::mat4>(1.0f), std::make_shared<glm::mat4>(1.0f) };
		//rob.CreateTextureMap(TexType::Albedo, "Textures/brick.jpg");
		//rob.CreateTextureMap(TexType::Metallic, "Textures/Metallic/brick.jpg");
		//rob.CreateTextureMap(TexType::Roughness, "Textures/Roughness/brick.jpg");
		//rob.CreateTextureMap(TexType::Normal, "Textures/Normal/brick.png");
		//rob.CreateTextureMap(TexType::Parallax, "Textures/Parallax/brick.jpg");
		//rob.CreateTextureMap(TexType::Glow, "Textures/Glow/brick.jpg");
		//renderObjDatas->push_back(std::make_shared<RenderObjectData>(rob));

		//rob = RenderObjectData{ std::make_shared<std::vector<Mesh>>(CreatePlane()), std::make_shared<std::map<TexType, std::vector<Texture>>>(), std::make_shared<glm::mat4>(1.0f), std::make_shared<glm::mat4>(1.0f) };
		//rob.CreateTextureMap(TexType::Albedo, "Textures/brick_floor.png");
		//rob.CreateTextureMap(TexType::Metallic, "Textures/Metallic/brick_floor.png");
		//rob.CreateTextureMap(TexType::Roughness, "Textures/Roughness/brick_floor.png");
		//rob.CreateTextureMap(TexType::Normal, "Textures/Normal/brick_floor.png");
		//rob.CreateTextureMap(TexType::Parallax, "Textures/Parallax/brick_floor.png");
		//rob.CreateTextureMap(TexType::Glow, "Textures/Glow/brick_floor.png");
		//renderObjDatas->push_back(std::make_shared<RenderObjectData>(rob));
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

		environmentMapShader 		= std::make_shared<Shader_Object>(std::vector<std::string>{"Shaders/cubemap.vert", "Shaders/equirectangular_to_cubemap.frag"});
		irradianceConvolutionShader = std::make_shared<Shader_Object>(std::vector<std::string>{"Shaders/cubemap.vert", "Shaders/irradiance_covolution.frag"});
		prefilterShader 			= std::make_shared<Shader_Object>(std::vector<std::string>{"Shaders/cubemap.vert", "Shaders/prefilter.frag"});
		brdfShader 					= std::make_shared<Shader_Object>(std::vector<std::string>{"Shaders/framebuffer.vert", "Shaders/brdf.frag"});
		dirShadowShader				= std::make_shared<Shader_Object>(std::vector<std::string>{"Shaders/directional_shadow_map.vert", "Shaders/directional_shadow_map.frag"});
		animDirShadowShader			= std::make_shared<Shader_Object>(std::vector<std::string>{"Shaders/anim_directional_shadow_map.vert", "Shaders/directional_shadow_map.frag"});
		terrDirShadowShader			= std::make_shared<Shader_Object>(std::vector<std::string>{"Shaders/terrain.vert", "Shaders/terrain.tessc", "Shaders/terrain_directional_shadow_map.tesse", "Shaders/directional_shadow_map.frag"});
		omniShadowShader			= std::make_shared<Shader_Object>(std::vector<std::string>{"Shaders/omni_shadow_map.vert", "Shaders/omni_shadow_map.geom", "Shaders/omni_shadow_map.frag"});
		animOmniShadowShader		= std::make_shared<Shader_Object>(std::vector<std::string>{"Shaders/anim_omni_shadow_map.vert", "Shaders/omni_shadow_map.geom", "Shaders/omni_shadow_map.frag"});
		//terrOmniShadowShader.CreateFromFiles("Shaders/terrain.vert", "Shaders/terrain.tessc", "Shaders/terrain_omni_directional_shadow_map.tesse", "Shaders/omni_shadow_map.geom", "Shaders/directional_shadow_map.frag");
		preZShader					= std::make_shared<Shader_Object>(std::vector<std::string>{"Shaders/depth_framebuffer.vert", "Shaders/depth_framebuffer.frag"});
		animPreZShader				= std::make_shared<Shader_Object>(std::vector<std::string>{"Shaders/anim_depth_framebuffer.vert", "Shaders/depth_framebuffer.frag"}); 
		terrPreZShader				= std::make_shared<Shader_Object>(std::vector<std::string>{"Shaders/terrain.vert", "Shaders/terrain.tessc", "Shaders/terrain_depth_framebuffer.tesse", "Shaders/depth_framebuffer.frag"});
		ssaoShader					= std::make_shared<Shader_Object>(std::vector<std::string>{"Shaders/ssao_framebuffer.vert", "Shaders/ssao_framebuffer.frag"});
		ssaoBlurShader				= std::make_shared<Shader_Object>(std::vector<std::string>{"Shaders/framebuffer.vert", "Shaders/ssao_blur_framebuffer.frag"});
		unrigShader					= std::make_shared<Shader_Object>(std::vector<std::string>{"Shaders/shader.vert", "Shaders/shader.frag"});
		rigShader					= std::make_shared<Shader_Object>(std::vector<std::string>{"Shaders/animated_shader.vert", "Shaders/shader.frag"});
		terrShader					= std::make_shared<Shader_Object>(std::vector<std::string>{"Shaders/terrain.vert", "Shaders/terrain.tessc", "Shaders/terrain.tesse", "Shaders/terrain.frag"});
		bloomShader					= std::make_shared<Shader_Object>(std::vector<std::string>{"Shaders/framebuffer.vert", "Shaders/blur_framebuffer.frag"});
		motionBlurShader			= std::make_shared<Shader_Object>(std::vector<std::string>{"Shaders/framebuffer.vert", "Shaders/motionBlur_framebuffer.frag"});
		exposureShader				= std::make_unique<Shader_Object>(std::vector<std::string>{"Shaders/framebuffer.vert", "Shaders/hdr_framebuffer.frag"});
		
		billboardShader				= std::make_unique<Shader_Object>(std::vector<std::string>{"Shaders/billboard.vert", "Shaders/billboard.frag"});

		particleShader->CreateFromFiles("Shaders/particles.vert", "Shaders/particles.frag");

		//ToDo: #20 simulation manager class
		//fluidFragShader->CreateFromFiles("Shaders/framebuffer.vert", "Shaders/2DFluid/fluid.frag");
		//fluidFragShader3D->CreateFromFiles("Shaders/3DFluid/fluid.vert", "Shaders/3DFluid/fluid.frag");
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
		return;
		//glUniform1f(uniformDispFactor, (0.2f * terrainScaleFactor1));

		//glm::mat4 model;
		//glm::mat4 prevPVM = glm::mat4();
		////model = glm::translate(model, glm::vec3(0.0f,-10.0f, 0.0f));
		////model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
		////model = glm::scale(model,glm::vec3(terrainScaleFactor, 1.0f, terrainScaleFactor));
		//glUniformMatrix4fv(uniformModel2, 1, GL_FALSE, glm::value_ptr(model));
		////prevPVM = camera->GetPreviousProjectionViewMatrix() * terrainList[0]->PrevMesh;
		////glUniformMatrix4fv(uniformPrevPVM2, 1, GL_FALSE, glm::value_ptr(prevPVM));
		//terrainTextureDisp->UseTexture(0);
		//if (shadow)
		//{
		//	terrainDirectionalShadowShader->SetDisplacementMap(1);
		//}
		//else if (depth)
		//{
		//	terrain_preZPassShader->SetDisplacementMap(1);
		//}
		//else
		//{
		//	terrainShader->SetDisplacementMap(1);
		//}
		//terrainShader->SetBlendMap(11);
		//terrainTexture->UseTextureArray(11);
		//terrainTextureMetal->UseTextureArray(12);
		//terrainTextureNorm->UseTextureArray(14);
		//terrainTextureRough->UseTextureArray(15);
		//terrainTexturePara->UseTextureArray(16);
		//dullTerrainMaterial->UseMaterial(uniformAlbedoMap2, uniformMetallicMap2, uniformNormalMap2, uniformRoughnessMap2, uniformParallaxMap2);

		//terrainList->at(0)->RenderMesh();
		////terrainList[0]->PrevMesh = model;
	}

	//Will be in Transform class or VObject class
	void Translate(const std::weak_ptr<Render_Object>& renderObj, glm::vec3 position)
	{
		if(auto ro = renderObj.lock())
		{
			if (auto model = ro->GetModelMatrix().lock())
			{
				*model = glm::translate(*model, position);
			}
		}
	}

	void RotateOnAxis(const std::weak_ptr<Render_Object>& renderObj, float angleInDegrees, glm::vec3 axis)
	{
		if (auto ro = renderObj.lock())
		{
			if (auto model = ro->GetModelMatrix().lock())
			{
				*model = glm::rotate(*model, angleInDegrees * toRadians, axis);
			}
		}
	}

	void Scale(const std::weak_ptr<Render_Object>& renderObj, glm::vec3 scale)
	{
		if (auto ro = renderObj.lock())
		{
			if (auto model = ro->GetModelMatrix().lock()) 
			{
				*model = glm::scale(*model, scale);
			}
		}
	}

	void Transform(const std::weak_ptr<Render_Object>& renderObj, const Transform& transform)
	{
		if (auto ro = renderObj.lock())
		{
			if (auto model = ro->GetModelMatrix().lock())
			{
				*model = glm::translate(*model, transform.Position);
				*model = glm::toMat4(transform.Rotation);
				*model = glm::scale(*model, transform.Scale);
			}
		}
	}

	void UpdateObjectTransforms() 
	{
		//ToDo: Temporary
		//for (auto obj : *vObjectPool)
		//{
		//	Transform(obj->render_object, *obj->transform);
		//}
		//pyramid1->Translate(-terrainScaleFactor, 34.0f, -2.5f - terrainScaleFactor);
		////pyramid1->Rotate(curAngle, 0.0f, 1.0f, 0.0f);
		////pyramid1->Scale(0.4f, 0.4f, 1.0f);

		//pyramid2->Translate(-terrainScaleFactor, 30.0f, -2.5f - terrainScaleFactor);
		////pyramid2->Rotate(curAngle, 0.0f, 1.0f, 0.0f);
		////pyramid2->Scale(0.4f, 0.4f, 1.0f);

		//rectangle1->Translate(-15.0f - terrainScaleFactor, 43.0f, -terrainScaleFactor);
		//rectangle1->Rotate(90, 1.0f, 0.0f, 0.0f);
		//rectangle1->Rotate(-90, 0.0f, 0.0f, 1.0f);

		//rectangle2->Translate(-terrainScaleFactor, 43.0f, -15.0f - terrainScaleFactor);
		//rectangle2->Rotate(-90, 1.0f, 0.0f, 0.0f);
		//rectangle2->Rotate(180.0f, 0.0f, 0.0f, 1.0f);

		//cube->Translate(curScale - terrainScaleFactor, 32.0f, 4.5f - terrainScaleFactor);
		////cube->Rotate(curAngle, 0.0f, 1.0f, 0.0f);
		//cube->Rotate(90.0f, 1.0f, 0.0f, 0.0f);
		//cube->Scale(0.1f, 0.1f, 0.1f);

		////sphere->Translate(-terrainScaleFactor, 35.0f, 5.5f - terrainScaleFactor);
		////sphere->Rotate(curAngle, 0.0f, 1.0f, 0.0f);
		//sphere->Scale(1.0f, 1.0f, 1.0f);

		//sniper->Translate(-terrainScaleFactor, 36.0f, 11.0f - terrainScaleFactor);
		////sniper->Rotate(curAngle, 0.0f, 1.0f, 0.0f);
		//sniper->Rotate(90.0f, 1.0f, 0.0f, 0.0f);
		//sniper->Rotate(180.0f, 1.0f, 0.0f, 0.0f);
		//sniper->Scale(0.5f, 0.5f, 0.5f);

		//gun->Translate(5.0f - terrainScaleFactor, 33.0f, 10.0f - terrainScaleFactor);
		////gun->Rotate(curAngle, 0.0f, 1.0f, 0.0f);
		//gun->Rotate(180.0f, 0.0f, 1.0f, 0.0f);
		//gun->Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
		//gun->Scale(0.02f, 0.02f, 0.02f);

		//bulbWhite->Translate(pointLights[0]->GetPosition().x, pointLights[0]->GetPosition().y, pointLights[0]->GetPosition().z);
		//bulbWhite->Scale(10.0f, 10.f, 10.0f);

		//bulbRed->Translate(pointLights[1]->GetPosition().x, pointLights[1]->GetPosition().y, pointLights[1]->GetPosition().z);
		//bulbRed->Scale(10.0f, 10.f, 10.0f);

		//anymodel->Translate(-terrainScaleFactor, 37.0f, 1.0f - terrainScaleFactor);
		//anymodel->Scale(1.0f, 1.0f, 1.0f);

		//model = glm::mat4();
		//model = glm::translate(model, glm::vec3(-6.0f - terrainScaleFactor, 28.2f, -5.0f - terrainScaleFactor));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
		//model = glm::scale(model, glm::vec3(0.1, 0.1f, 0.1f));
		//glUniformMatrix4fv(uniformModel1, 1, GL_FALSE, glm::value_ptr(model));
		//prevPVM = camera->GetPreviousProjectionViewMatrix() * anim->prevModel;
		//glUniformMatrix4fv(uniformPrevPVM1, 1, GL_FALSE, glm::value_ptr(prevPVM));
		//shinyMaterialGlow->UseMaterial(uniformAlbedoMap1, uniformMetallicMap1, uniformNormalMap1, uniformRoughnessMap1, uniformParallaxMap1, uniformGlowMap1);

		//if (shadow)
		//{
		//	anim->UpdateBoneData(animDirectionalShadowShader->GetShaderID());
		//}
		//else if (depth)
		//{
		//	anim->UpdateBoneData(anim_preZPassShader->GetShaderID());
		//}
		//else {
		//	anim->UpdateBoneData(animShaderList[0]->GetShaderID());
		//}
		//anim->RenderModel();
		//anim->prevModel = model;

		//model = glm::mat4();
		//model = glm::translate(model, glm::vec3(6.0f - terrainScaleFactor, 28.2f, -5.0f - terrainScaleFactor));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
		//model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		//glUniformMatrix4fv(uniformModel1, 1, GL_FALSE, glm::value_ptr(model));
		//prevPVM = camera->GetPreviousProjectionViewMatrix() * anim2->prevModel;
		//glUniformMatrix4fv(uniformPrevPVM1, 1, GL_FALSE, glm::value_ptr(prevPVM));
		//shinyMaterialGlow->UseMaterial(uniformAlbedoMap1, uniformMetallicMap1, uniformNormalMap1, uniformRoughnessMap1, uniformParallaxMap1, uniformGlowMap1);

		//if (shadow)
		//{
		//	anim2->UpdateBoneData(animDirectionalShadowShader->GetShaderID());
		//}
		//else if (depth)
		//{
		//	anim2->UpdateBoneData(anim_preZPassShader->GetShaderID());
		//}
		//else
		//{
		//	anim2->UpdateBoneData(animShaderList[0]->GetShaderID());
		//}

		//anim2->RenderModel();
		//anim2->prevModel = model;
	}

	void DirectionalShadowMapPass(DirectionalLight* light, const CamParam& camParam) {

		auto testLitView = light->CalculateCascadeLightTransform();
		light->CalcOrthProjs(camera->CalculateViewMatrix(), &testLitView, 60.0f);

		auto direction = light->GetLightDirection();

		glm::mat4 proj[NUM_CASCADES];

		for (auto i = 0; i < NUM_CASCADES; ++i)
		{
			vView[i] = glm::lookAt(mainLight->GetModlCent(i), light->GetModlCent(i) + glm::normalize(direction) * 0.2f, light->GetLightUp());
			proj[i] = light->GetProjMat(vView[i], i);
		}

		auto lightParam = LightParam{ nullptr, proj, vView, &direction };
		dirShadowRPHandler->Update(*sceneObjRO, &camParam, &lightParam);
	}

	void OmniShadowMapPass(const std::vector<std::shared_ptr<PointLight>>& lights) 
	{
		std::vector<glm::vec3> positions;
		std::vector<glm::mat4> projections;
		std::vector<GLfloat> farplanes;

		for (auto i = 0; i < lights.size(); ++i) 
		{
			positions.push_back(lights[i]->position);
			projections.push_back(lights[i]->lightProj);
			farplanes.push_back(lights[i]->farPlane);
		}

		auto lightParam = LightParam(&positions[0], &projections[0], nullptr, nullptr, &farplanes[0], nullptr, lights.size());
		omniShadowRPHandler->Update(*sceneObjRO, nullptr, &lightParam);
	}

	void CullLight()
	{
		visibleClusterCompShader->UseShader();
		depthMap->AttachFBOToTextureUnit(0, GL_TEXTURE0, 0, 0);
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

	void PreZPass(const CamParam& camParam)
	{
		preZRPHandler->Update(*sceneObjRO, &camParam);
	}

	void SSAOPass(const CamParam& camParam)
	{
		auto ssaoTexData = std::map<TexType, std::vector<std::weak_ptr<Texture>>>();
		ssaoTexData.emplace(Noise, std::vector<std::weak_ptr<Texture>>{SSAONoiseTexture});
		quadRO->at(0)[0]->SetTextures(std::move(ssaoTexData));
		ssaoRPHandler->Update(*quadRO, &camParam);
		quadRO->at(0)[0]->ResetTextures();
	}

	void SSAOBlurPass(const CamParam& camParam)
	{
		ssaoBlurRPHandler->Update(*quadRO, &camParam);
	}

	void RenderPass(const CamParam& camParam, DirectionalLight* dirlight, PointLight* pointLights, SpotLight* spotLights)
	{
		auto direction = dirlight->GetLightDirection();
		auto dirColor = dirlight->color;
		glm::mat4 proj[NUM_CASCADES];

		for (auto i = 0; i < NUM_CASCADES; ++i)
		{
			proj[i] = dirlight->GetProjMat(vView[i], i);
		}

		auto lightParams = std::vector<LightParam>();
		auto lightParam = LightParam{ nullptr, proj, vView, &direction, nullptr, nullptr, 1, &dirColor };
		lightParams.push_back(lightParam);

		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> directions;
		std::vector<glm::mat4> projections;
		std::vector<GLfloat> farplanes;
		std::vector<GLfloat> edges;
		std::vector<glm::vec3> colors;

		for (auto i = 0; i < pointLightCount; ++i)
		{
			positions.push_back(pointLights[i].position);
			projections.push_back(pointLights[i].lightProj);
			farplanes.push_back(pointLights[i].farPlane);
			colors.push_back(pointLights[i].color);
		}

		lightParam = LightParam(&positions[0], &projections[0], nullptr, nullptr, &farplanes[0], nullptr, pointLightCount, &colors[0]);
		lightParams.push_back(lightParam);

		positions.clear();
		directions.clear();
		projections.clear();
		farplanes.clear();
		edges.clear();
		colors.clear();

		for (auto i = 0; i < spotLightCount; ++i)
		{
			positions.push_back(spotLights[i].position);
			directions.push_back(spotLights[i].direction);
			projections.push_back(spotLights[i].lightProj);
			farplanes.push_back(spotLights[i].farPlane);
			edges.push_back(spotLights[i].procEdge);
			colors.push_back(spotLights[i].color);
		}

		lightParam = LightParam(&positions[0], &projections[0], nullptr, &directions[0], &farplanes[0], &edges[0], spotLightCount, &colors[0]);
		lightParams.push_back(lightParam);

		sceneRPHandler->Update(*sceneObjRO, &camParam, &lightParams[0]);
	}

	void Bloom()
	{
		bloomRPHandler->Update(*quadRO);
	}

	void MotionBlurPass(const CamParam& camParam)
	{
		motionBlurRPHandler->Update(*quadRO, &camParam);
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
			exposureRPHandler->Update(*quadRO);
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
	engineUI->AddSceneViewers(Pimpl()->ssaoBlurFbo->GetFBOBuffer(0, 0), "EditorScene", Editor, [this](bool isSelected) { Pimpl()->isEditorViewSelected = isSelected; });
	engineUI->AddSceneViewers(Pimpl()->sceneFbo->GetFBOBuffer(0, 0), "InGameScene", InGame, [this](bool isSelected) { Pimpl()->mainWindow->SetCursorActive(!isSelected); Pimpl()->isGameViewSelected = isSelected; });
}

bool RenderEngineMain:: IsEnd()
{
	return Pimpl()->mainWindow->getShouldClose();
}

RenderEngineMain::~RenderEngineMain() = default;