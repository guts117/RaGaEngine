#include "render_pch.h"
#include "RenderEngineMain.h"

#include "Mesh.h"
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

#include "Debug.h"

#include "RenderingCommonValues.h"
#include "MathUtil.h"

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
#include "Skybox_Render_Pass_Handler.h"
#include "Billboard_Render_Pass_Handler.h"

#include <glm/gtx/euler_angles.hpp>
#include <regex>

using namespace NarakaRenderEngine;
using namespace RenderEngine;

struct RenderEngineMain::Impl
{
	Impl() = default;

	Impl(Impl&& rhs) noexcept = delete;
	Impl& operator=(Impl&& rhs) noexcept = delete;

	Impl(const Impl& rhs) noexcept = delete;
	Impl& operator=(const Impl& rhs) noexcept = delete;

	bool drawFluidSim = false;
	bool drawSmokeSim = false;

	float terrainScaleFactor1 = 1000.0f;

	const float toRadians = static_cast<float>(M_PI) / 180.0f;

	std::shared_ptr <Shader_Object> buildAABBGridCompShader;
	std::shared_ptr <Shader_Object> visibleClusterCompShader;
	std::shared_ptr <Shader_Object> cullLightsCompShader;

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

	std::unique_ptr<Scene_Fbo_Handler_Manager> m_SceneFboHandlerMgr;
	Fbo_Handler* depthMap;
	Fbo_Handler* exposureFbo;
	Fbo_Handler* cameraBlitFbo;

	//ToDo: #62
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

	ClusteringMemoryPool<Texture> texturePool = ClusteringMemoryPool<Texture>(5);
	ClusteringMemoryPool<Mesh> meshPool = ClusteringMemoryPool<Mesh>(5);
	ClusteringMemoryPool<Transform> modelMatrixPool = ClusteringMemoryPool<Transform>(5);
	ClusteringMemoryPool<Transform> prevModelMatrixPool = ClusteringMemoryPool<Transform>(5);
	ClusteringMemoryPool<Shader_Object> shaderObjPool = ClusteringMemoryPool<Shader_Object>(5);

	std::vector<std::vector<Render_Object>> ssaoQuadRO;
	std::vector<std::vector<Render_Object>> quadRO;
	std::vector<std::vector<Render_Object>> cwCubeRO;
	std::vector<std::vector<Render_Object>> sceneObjRO;
	std::vector<std::vector<Render_Object>> billboardRO;

	rw_clustering_ptr<Mesh> AddToMeshPool(Mesh&& mesh)
	{
		return meshPool.AddToPool(std::move(mesh));
	}
	rw_clustering_ptr<Transform> AddToModelMatrixPool(glm::mat4&& mat)
	{
		auto trns = Transform();
		trns.SetModelMatrtix(mat);
		return modelMatrixPool.AddToPool(std::move(trns));
	}
	rw_clustering_ptr<Transform> AddToPrevModelMatrixPool(glm::mat4&& mat)
	{
		auto trns = Transform();
		trns.SetModelMatrtix(mat);
		return prevModelMatrixPool.AddToPool(std::move(trns));
	}

	std::map<TexType, std::vector<rw_clustering_ptr<Texture>>> CreateTextureMap(std::vector<TexMapData>&& texMapData, const std::vector<glm::vec3>& color = std::vector<glm::vec3>())
	{
		auto textureMap = std::map<TexType, std::vector<rw_clustering_ptr<Texture>>>();

		for(auto i = 0; i< texMapData.size(); ++i)
		{
			TexMapData& dat = texMapData[i];
			auto hasLoaded = false;
			auto texture = Texture();
			switch (dat.type)
			{
				case TexType::HDR:
					texture = Texture(std::move(dat.path));
					hasLoaded = texture.LoadTextureHDR();
					break;
				case TexType::Noise:
					hasLoaded = texture.LoadNativeTexture(color);
					break;
				default:
					texture = Texture(std::move(dat.path), dat.type == TexType::Albedo || TexType::Default);
					hasLoaded = texture.LoadTexture2D();
					break;
			}

			auto texPtr = texturePool.AddToPool(std::move(texture));

			if(hasLoaded)
			{
				if (textureMap.contains(dat.type))
				{
					textureMap.at(dat.type).emplace_back(texPtr);
				}
				else
				{
					auto vec = std::vector<rw_clustering_ptr<Texture>>();
					vec.emplace_back(texPtr);
					textureMap.emplace(dat.type, vec);
				}
			}
		}

		return textureMap;
	}

	rw_clustering_ptr<Shader_Object> AddToShaderObjectPool(Shader_Object&& shaderObj)
	{	
		return shaderObjPool.AddToPool(std::move(shaderObj));
	}

	std::vector< std::shared_ptr < ParticleSystem>> particleList;

	//std::vector<Mesh*> quad;
	//std::vector<Mesh*> cwCube;
	//Mesh ccw_cube;

	std::vector<std::shared_ptr<Camera>> cameras;

	std::shared_ptr < DirectionalLight> mainLight;
	std::shared_ptr < PointLight> pointLights[MAX_POINT_LIGHTS_WITH_SHADOW];
	std::shared_ptr < SpotLight> spotLights[MAX_SPOT_LIGHTS];
	std::shared_ptr <std::vector<std::shared_ptr<PointLight>>> omniDirLights;

	unsigned int pointLightCount = 0;
	unsigned int spotLightCount = 0;

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
 
	//Texture environmentTexture;
	//Texture SSAONoiseTexture;
	//Texture plainTexture;

	//Render_Object pyramid1;
	//Render_Object pyramid2;
	//Render_Object rectangle1;
	//Render_Object rectangle2;

	//Render_Object sniper;
	//Render_Object gun;
	//Render_Object anymodel;
	//Render_Object cube;
	//Render_Object sphere;
	//Render_Object bulbWhite;
	//Render_Object bulbRed;

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
	std::shared_ptr<Skybox_Render_Pass_Handler> skyboxRPHandler;
	std::shared_ptr<Billboard_Render_Pass_Handler> billBoardRPHandler;

	int isFocusedCount;

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

	void Init(const glm::ivec2& screenDims)
	{
		glEnable(GL_TEXTURE_3D);
		lastFrameTime = static_cast<GLfloat>(glfwGetTime());
		//CreateBillboard();
		//CreateParticles();

		sceneObjRO = std::vector<std::vector<Render_Object>>();
		billboardRO = std::vector<std::vector<Render_Object>>();

		sceneObjRO.reserve(100);
		billboardRO.reserve(100);

		//ToDo: Expand This on a dedicated issue #61
		//CreateTerrain();
		CreateObject();
		CreateShaders();

		auto environmentMapShader = AddToShaderObjectPool(Shader_Object(std::vector<std::string>{"Shaders/cubemap.vert", "Shaders/equirectangular_to_cubemap.frag"}));
		auto irradianceConvolutionShader = AddToShaderObjectPool(Shader_Object(std::vector<std::string>{"Shaders/cubemap.vert", "Shaders/irradiance_covolution.frag"}));
		auto prefilterShader = AddToShaderObjectPool(Shader_Object(std::vector<std::string>{"Shaders/cubemap.vert", "Shaders/prefilter.frag"}));
		auto brdfShader = AddToShaderObjectPool(Shader_Object(std::vector<std::string>{"Shaders/framebuffer.vert", "Shaders/brdf.frag"}));
		auto dirShadowShader = AddToShaderObjectPool(Shader_Object(std::vector<std::string>{"Shaders/directional_shadow_map.vert", "Shaders/directional_shadow_map.frag"}));
		auto animDirShadowShader = AddToShaderObjectPool(Shader_Object(std::vector<std::string>{"Shaders/anim_directional_shadow_map.vert", "Shaders/directional_shadow_map.frag"}));
		auto terrDirShadowShader = AddToShaderObjectPool(Shader_Object(std::vector<std::string>{"Shaders/terrain.vert", "Shaders/terrain.tessc", "Shaders/terrain_directional_shadow_map.tesse", "Shaders/directional_shadow_map.frag"}));
		auto omniShadowShader = AddToShaderObjectPool(Shader_Object(std::vector<std::string>{"Shaders/omni_shadow_map.vert", "Shaders/omni_shadow_map.geom", "Shaders/omni_shadow_map.frag"}));
		auto animOmniShadowShader = AddToShaderObjectPool(Shader_Object(std::vector<std::string>{"Shaders/anim_omni_shadow_map.vert", "Shaders/omni_shadow_map.geom", "Shaders/omni_shadow_map.frag"}));
		//terrOmniShadowShader.CreateFromFiles("Shaders/terrain.vert", "Shaders/terrain.tessc", "Shaders/terrain_omni_directional_shadow_map.tesse", "Shaders/omni_shadow_map.geom", "Shaders/directional_shadow_map.frag");
		auto preZShader = AddToShaderObjectPool(Shader_Object(std::vector<std::string>{"Shaders/depth_framebuffer.vert", "Shaders/depth_framebuffer.frag"}));
		auto animPreZShader = AddToShaderObjectPool(Shader_Object(std::vector<std::string>{"Shaders/anim_depth_framebuffer.vert", "Shaders/depth_framebuffer.frag"}));
		auto terrPreZShader = AddToShaderObjectPool(Shader_Object(std::vector<std::string>{"Shaders/terrain.vert", "Shaders/terrain.tessc", "Shaders/terrain_depth_framebuffer.tesse", "Shaders/depth_framebuffer.frag"}));
		auto ssaoShader = AddToShaderObjectPool(Shader_Object(std::vector<std::string>{"Shaders/ssao_framebuffer.vert", "Shaders/ssao_framebuffer.frag"}));
		auto ssaoBlurShader = AddToShaderObjectPool(Shader_Object(std::vector<std::string>{"Shaders/framebuffer.vert", "Shaders/ssao_blur_framebuffer.frag"}));
		auto unrigShader = AddToShaderObjectPool(Shader_Object(std::vector<std::string>{"Shaders/shader.vert", "Shaders/shader.frag"}));
		auto rigShader = AddToShaderObjectPool(Shader_Object(std::vector<std::string>{"Shaders/animated_shader.vert", "Shaders/shader.frag"}));
		auto terrShader = AddToShaderObjectPool(Shader_Object(std::vector<std::string>{"Shaders/terrain.vert", "Shaders/terrain.tessc", "Shaders/terrain.tesse", "Shaders/terrain.frag"}));
		auto bloomShader = AddToShaderObjectPool(Shader_Object(std::vector<std::string>{"Shaders/framebuffer.vert", "Shaders/blur_framebuffer.frag"}));
		auto motionBlurShader = AddToShaderObjectPool(Shader_Object(std::vector<std::string>{"Shaders/framebuffer.vert", "Shaders/motionBlur_framebuffer.frag"}));
		auto exposureShader = AddToShaderObjectPool(Shader_Object(std::vector<std::string>{"Shaders/framebuffer.vert", "Shaders/hdr_framebuffer.frag"}));
		auto skyboxShader = AddToShaderObjectPool(Shader_Object(std::vector<std::string>{"Shaders/skybox.vert", "Shaders/skybox.frag"}));
		auto billboardShader = AddToShaderObjectPool(Shader_Object(std::vector<std::string>{"Shaders/billboard.vert", "Shaders/billboard.frag"}));
		auto particleShader = AddToShaderObjectPool(Shader_Object(std::vector<std::string>{"Shaders/particles.vert", "Shaders/particles.frag"}));

		auto gameCamera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f, 50.0f, 0.2f);
		cameras.push_back(gameCamera);
		auto editorCamera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 50.0f, 0.2f, true);
		cameras.push_back(editorCamera);

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

		m_SceneFboHandlerMgr = std::make_unique<Scene_Fbo_Handler_Manager>("InGame", screenDims);

		auto environmentMap = m_SceneFboHandlerMgr->FindFboHandler("Environment_Map_Pass");
		auto irradianceMap = m_SceneFboHandlerMgr->FindFboHandler("Irradiance_Map_Pass");
		auto prefilterMap = m_SceneFboHandlerMgr->FindFboHandler("Pre_Filter_Pass");
		auto brdfMap = m_SceneFboHandlerMgr->FindFboHandler("Brdf_Pass");
		depthMap = m_SceneFboHandlerMgr->FindFboHandler("Depth_Pass");
		auto ssaoFbo = m_SceneFboHandlerMgr->FindFboHandler("Ssao_Pass");
		auto ssaoBlurFbo = m_SceneFboHandlerMgr->FindFboHandler("Ssao_Blur_Pass");
		auto sceneFbo = m_SceneFboHandlerMgr->FindFboHandler("Shading_Pass");
		auto motionBlurFbo = m_SceneFboHandlerMgr->FindFboHandler("Motion_Blur_Pass");
		auto bloomFbo = m_SceneFboHandlerMgr->FindFboHandler("Bloom_Pass");
		exposureFbo = m_SceneFboHandlerMgr->FindFboHandler("Final_Output_Pass");
		auto omniShadowMaps = m_SceneFboHandlerMgr->FindFboHandler("Omni_Shadow_Map_Pass");

		cameraBlitFbo = m_SceneFboHandlerMgr->AddGameCameraFboHandlers(0, screenDims);

		auto quad = std::vector<rw_clustering_ptr<Mesh>>();

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

		auto quadIndicesSsao = quadIndices;
		auto quadVerticesSsao = quadVertices;

		auto qMesh = Mesh(0, std::move(quadVertices), std::move(quadIndices), std::move(MeshGenParams()));	
		quad.push_back(AddToMeshPool(std::move(qMesh)));
		auto quadInVec = std::vector<Render_Object>();
		quadInVec.emplace_back(Render_Object(std::move(quad)));
		quadRO = std::vector<std::vector<Render_Object>>();
		quadRO.emplace_back(std::move(quadInVec));
		quad.clear();

		qMesh = Mesh(0, std::move(quadVerticesSsao), std::move(quadIndicesSsao), std::move(MeshGenParams()));
		quad.push_back(AddToMeshPool(std::move(qMesh)));
		quadInVec = std::vector<Render_Object>();
		quadInVec.emplace_back(Render_Object(std::move(quad)));
		ssaoQuadRO = std::vector<std::vector<Render_Object>>();
		ssaoQuadRO.emplace_back(std::move(quadInVec));

		auto cwCube = std::vector<rw_clustering_ptr<Mesh>>();
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

		auto cwMesh = Mesh(0, std::move(cwCubeVertices), std::move(cwCubeIndices), std::move(MeshGenParams()));
		cwCube.push_back(AddToMeshPool(std::move(cwMesh)));
		auto cwCubeInVec = std::vector<Render_Object>();
		cwCubeInVec.emplace_back(Render_Object(std::move(cwCube)));
		cwCubeRO = std::vector<std::vector<Render_Object>>();
		cwCubeRO.emplace_back(std::move(cwCubeInVec));

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
		//ccw_cube = Mesh(0, std::move(ccwCubeVertices), std::move(ccwCubeIndices), std::move(MeshGenParams()));

		mainLight = std::make_unique < DirectionalLight>(1024, 1024,
			0.5f, 0.5f, 0.5f,
			5500.0f, -5500.0f, -10000.0f,
			m_SceneFboHandlerMgr.get());

		pointLights[0] = std::make_unique < PointLight>(512, 512,
			0.1f, 100.0f,
			0.0f, 0.0f, 3.0f,
			12.0f, 5.0f, 10.0f,
			m_SceneFboHandlerMgr.get());

		pointLightCount++;

		pointLights[1] = std::make_unique < PointLight>(512, 512,
			0.1f, 100.0f,
			3.0f, 0.0f, 0.0f,
			-12.0f, 5.0f, 10.0f,
			m_SceneFboHandlerMgr.get());

		pointLightCount++;

		spotLights[0] = std::make_unique < SpotLight>(512, 512,
			0.1f, 100.0f,
			10.0f, 10.0f, 10.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			10.0f, m_SceneFboHandlerMgr.get());

		spotLightCount++;

		omniDirLights = std::make_shared<std::vector<std::shared_ptr<PointLight>>>();
		for (auto i = 0; i < pointLightCount; ++i){ omniDirLights->push_back(pointLights[i]); }
		for (auto i = 0; i < spotLightCount; ++i) { omniDirLights->push_back(spotLights[i]); }

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

		InitSSBOs(screenDims);
		CreateClusters();

		//texMapDatas = std::vector<TexMapData>();
		//texMapDatas.push_back(TexMapData{ TexType::Default,		"Textures/plain.png" });
		//CreateTextureMap(std::move(texMapDatas));

		auto envMapShaders = std::vector<rw_clustering_ptr<Shader_Object>>{ environmentMapShader };
		envMapRPHandler = std::make_shared<Environment_Map_Render_Pass_Handler>(environmentMap, std::move(envMapShaders));
		auto texMapDatas = std::vector<TexMapData>();
		texMapDatas.push_back(TexMapData{ TexType::HDR,		"Textures/HDR/syferfontein_0d_clear_puresky_4k.hdr" });	
		auto envMapTexData = CreateTextureMap(std::move(texMapDatas));
		cwCubeRO[0][0].SetTextures(std::move(envMapTexData));
		envMapRPHandler->Update(cwCubeRO);
		cwCubeRO[0][0].ResetTextures();

		auto irrConvShaders = std::vector<rw_clustering_ptr<Shader_Object>>{ irradianceConvolutionShader };
		auto inputs = std::make_shared<std::vector<std::shared_ptr<std::any>>>();
		inputs->push_back(std::make_shared<std::any>(std::make_any<Fbo_Handler*>(environmentMap)));
		irrConvRPHandler = std::make_shared<Irradiance_Convolution_Render_Pass_Handler>(irradianceMap, std::move(irrConvShaders), inputs);
		irrConvRPHandler->Update(cwCubeRO);

		auto prefilterShaders = std::vector<rw_clustering_ptr<Shader_Object>>{ prefilterShader };
		prefilterRPHandler = std::make_shared<Prefilter_Render_Pass_Handler>(prefilterMap, std::move(prefilterShaders), inputs);
		prefilterRPHandler->Update(cwCubeRO);

		auto brdfShaders = std::vector<rw_clustering_ptr<Shader_Object>>{ brdfShader };
		brdfRPHandler = std::make_shared<Brdf_Render_Pass_Handler>(brdfMap, std::move(brdfShaders));
		brdfRPHandler->Update(quadRO);

		auto dirShadowShaders = std::vector<rw_clustering_ptr<Shader_Object>>{ dirShadowShader, animDirShadowShader, terrDirShadowShader };
		dirShadowRPHandler = std::make_shared<Directional_Shadow_Map_Render_Pass_Handler>(mainLight->GetShadowMap(), std::move(dirShadowShaders));

		auto ommiShadowShaders = std::vector<rw_clustering_ptr<Shader_Object>>{ omniShadowShader, animOmniShadowShader };
		omniShadowRPHandler = std::make_shared<Omni_Directional_Shadow_Map_Render_Pass_Handler>(omniShadowMaps, std::move(ommiShadowShaders));

		auto prezShaders = std::vector<rw_clustering_ptr<Shader_Object>>{ preZShader, animPreZShader, terrPreZShader};
		preZRPHandler = std::make_shared<PreZ_Render_Pass_Handler>(depthMap, std::move(prezShaders));

		auto ssaoShaders = std::vector<rw_clustering_ptr<Shader_Object>>{ ssaoShader };
		inputs = std::make_shared<std::vector<std::shared_ptr<std::any>>>();
		inputs->push_back(std::make_shared<std::any>(std::make_any<Fbo_Handler*>(depthMap)));
		ssaoRPHandler = std::make_shared<Ssao_Render_Pass_Handler>(ssaoFbo, std::move(ssaoShaders), inputs);
		InitSSAO();

		auto ssaoBlurShaders = std::vector<rw_clustering_ptr<Shader_Object>>{ ssaoBlurShader };
		inputs = std::make_shared<std::vector<std::shared_ptr<std::any>>>();
		inputs->push_back(std::make_shared<std::any>(std::make_any<Fbo_Handler*>(ssaoFbo)));
		ssaoBlurRPHandler = std::make_shared<Ssao_Blur_Render_Pass_Handler>(ssaoBlurFbo, std::move(ssaoBlurShaders), inputs);

		auto sceneShaders = std::vector<rw_clustering_ptr<Shader_Object>>{ unrigShader, rigShader, terrShader};
		inputs = std::make_shared<std::vector<std::shared_ptr<std::any>>>();
		inputs->push_back(std::make_shared<std::any>(std::make_any<Fbo_Handler*>(mainLight->GetShadowMap())));
		inputs->push_back(std::make_shared<std::any>(std::make_any<Fbo_Handler*>(omniShadowMaps)));
		inputs->push_back(std::make_shared<std::any>(std::make_any<Fbo_Handler*>(irradianceMap)));
		inputs->push_back(std::make_shared<std::any>(std::make_any<Fbo_Handler*>(prefilterMap)));
		inputs->push_back(std::make_shared<std::any>(std::make_any<Fbo_Handler*>(brdfMap)));
		inputs->push_back(std::make_shared<std::any>(std::make_any<Fbo_Handler*>(ssaoBlurFbo)));
		inputs->push_back(std::make_shared<std::any>(std::make_any<Fbo_Handler*>(depthMap)));
		sceneRPHandler = std::make_shared<Scene_Render_Pass_Handler>(sceneFbo, std::move(sceneShaders), inputs);

		auto bloomShaders = std::vector<rw_clustering_ptr<Shader_Object>>{ bloomShader };
		inputs = std::make_shared<std::vector<std::shared_ptr<std::any>>>();
		inputs->push_back(std::make_shared<std::any>(std::make_any<Fbo_Handler*>(sceneFbo)));
		bloomRPHandler = std::make_shared<Bloom_Render_Pass_Handler>(bloomFbo, std::move(bloomShaders), inputs);

		auto motionBlurShaders = std::vector<rw_clustering_ptr<Shader_Object>>{ motionBlurShader };
		inputs = std::make_shared<std::vector<std::shared_ptr<std::any>>>();
		inputs->push_back(std::make_shared<std::any>(std::make_any<Fbo_Handler*>(sceneFbo)));
		motionBlurRPHandler = std::make_shared<Motion_Blur_Render_Pass_Handler>(motionBlurFbo, std::move(motionBlurShaders), inputs);

		auto exosureShaders = std::vector<rw_clustering_ptr<Shader_Object>>{ exposureShader };
		inputs = std::make_shared<std::vector<std::shared_ptr<std::any>>>();
		inputs->push_back(std::make_shared<std::any>(std::make_any<Fbo_Handler*>(bloomFbo)));
		inputs->push_back(std::make_shared<std::any>(std::make_any<Fbo_Handler*>(motionBlurFbo)));
		exposureRPHandler = std::make_shared<Exposure_Render_Pass_Handler>(exposureFbo, std::move(exosureShaders), inputs);

		auto skyboxShaders = std::vector<rw_clustering_ptr<Shader_Object>>{ skyboxShader };
		inputs = std::make_shared<std::vector<std::shared_ptr<std::any>>>();
		inputs->push_back(std::make_shared<std::any>(std::make_any<Fbo_Handler*>(environmentMap)));
		skyboxRPHandler = std::make_shared<Skybox_Render_Pass_Handler>(sceneFbo, std::move(skyboxShaders), inputs);

		auto billboardShaders = std::vector<rw_clustering_ptr<Shader_Object>>{ billboardShader };
		billBoardRPHandler = std::make_shared<Billboard_Render_Pass_Handler>(sceneFbo, std::move(billboardShaders));
	
/*		modelMatrixPool.ExecuteClusteredTasks();
		prevModelMatrixPool.ExecuteClusteredTasks();
		texturePool.ExecuteClusteredTasks();
		shaderObjPool.ExecuteClusteredTasks();
		meshPool.ExecuteClusteredTasks()*/;
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

		auto texMapDatas = std::vector<TexMapData>();
		texMapDatas.push_back(TexMapData{ TexType::Noise, "" });
		auto ssaoTexData = CreateTextureMap(std::move(texMapDatas), ssaoNoiseData);
		ssaoQuadRO[0][0].SetTextures(std::move(ssaoTexData));
	}
	
	void UpdateAtFrameBufferResize(const glm::ivec2& screenDims)
	{
		auto invProj = glm::inverse(cameras[0]->GetProjectionMatrix(screenDims));
		glNamedBufferSubData(screenToViewSSBO, 0, sizeof(invProj), &invProj);
		sizeX = (unsigned int)std::ceilf(screenDims.x / (float)gridSizeX);
		sizeY = (unsigned int)std::ceilf(screenDims.y / (float)gridSizeY);
		int data[4] = { sizeX, sizeY, screenDims.x, screenDims.y };
		glNamedBufferSubData(screenToViewSSBO, 80, sizeof(data), &data);

		m_SceneFboHandlerMgr->ResizeScreenFboHandlers(screenDims.x, screenDims.y);
		CreateClusters();
	}

	void Update(const glm::ivec2& screenDims, const bool& isUpdateBuffers)
	{
		if(isUpdateBuffers)
		{
			UpdateAtFrameBufferResize(screenDims);
		}

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

		if (!drawFluidSim && !drawSmokeSim)
		{		
			for(auto& camera: cameras)
			{
				auto camParam = CamParam{ camera->getCameraPosition(), camera->GetProjectionMatrix(screenDims), camera->CalculateViewMatrix()
						, camera->GetPreviousProjectionMatrix(), camera->GetPreviousViewMatrix(), camera->GetPreviousProjectionViewMatrix()
						, framesPerSec
						, camera->getCameraUp()
						, camera->getCameraRight() };

				DirectionalShadowMapPass(mainLight.get(), camParam, screenDims);
				OmniShadowMapPass(*omniDirLights, camParam);

				PreZPass(camParam);
				CullLight(camParam, screenDims);
				SSAOPass(camParam);
				SSAOBlurPass(camParam);
				RenderPass(camParam, mainLight.get(), pointLights, spotLights);
				Bloom();
				MotionBlurPass(camParam);
				exposureRPHandler->Update(quadRO);

				if (!camera->isEditor)
				{
					exposureFbo->Blit(0, cameraBlitFbo, 0);
					auto lowerLight = camParam.Position;
					lowerLight.y -= 0.1f;
					spotLights[0]->SetFlash(lowerLight, camera->getCameraDirection());
				}

				camera->UpdatePreviousMatrices(camParam.View, camParam.Projection);
			}
		}

		glUseProgram(0);

		//modelMatrixPool.ExecuteClusteredTasksSerial();
		//prevModelMatrixPool.ExecuteClusteredTasksSerial();
		//texturePool.ExecuteClusteredTasksSerial();
		//shaderObjPool.ExecuteClusteredTasksSerial();
		//meshPool.ExecuteClusteredTasksSerial();
	}

	void EndUpdate() 
	{
	}

	void InitSSBOs(const glm::ivec2& screenDims)
	{
		//Setting up tile size on both X and Y 
		sizeX = (unsigned int)std::ceilf(screenDims.x / (float)gridSizeX);
		sizeY = (unsigned int)std::ceilf(screenDims.y / (float)gridSizeY);

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
			screen2View.inverseProjectionMat = glm::inverse(cameras[0]->GetProjectionMatrix(screenDims));

			screen2View.tileSizes[0] = gridSizeX;
			screen2View.tileSizes[1] = gridSizeY;
			screen2View.tileSizes[2] = gridSizeZ;
			screen2View.tileSizes[3] = maxLightsPerTile;
			screen2View.tileSizeInPixel[0] = sizeX;
			screen2View.tileSizeInPixel[1] = sizeY;
			screen2View.screenWidth = screenDims.x;
			screen2View.screenHeight = screenDims.y;
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
		buildAABBGridCompShader->UseShaderObject();
		buildAABBGridCompShader->DispatchShaderObject(glm::uvec3(1, 1, gridSizeZ));
	}

	Mesh CreateBillboard() 
	{
		//Show the front face to the camera
		std::vector<GLuint> billboardIndices = {
			0, 1, 2,
			1, 3, 2
		};

		std::vector<std::vector<GLfloat>> billboardVertices = 
		{
			std::vector<GLfloat>{	-0.5f, -0.5f, 0.0f		},
			std::vector<GLfloat>{	0.5f, -0.5f, 0.0f,		},
			std::vector<GLfloat>{	-0.5f, 0.5f, 0.0f,		},
			std::vector<GLfloat>{	0.5f, 0.5f, 0.0f,		}
		};
		return Mesh(0, std::move(billboardVertices), std::move(billboardIndices), std::move(MeshGenParams{ }));
	}

	void CreateParticles()
	{
		unsigned int particlesIndices[] = {
			0, 2, 1,
			1, 2, 3
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

	//ToDo: Expand This on a dedicated issue #61
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

	Mesh CreatePrism()
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

		return Mesh(0, std::move(vertices), std::move(indices), std::move(MeshGenParams{ true, true }));
	}

	Mesh CreatePlane()
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

		return Mesh(0, std::move(vertices), std::move(indices), std::move(MeshGenParams{ true, true }));
	}

	void CreateObject() 
	{	
		auto unrigStuff = std::vector<Render_Object>();

		auto mesh0 = CreatePrism();
		auto texMapDatas = std::vector<TexMapData>();
		texMapDatas.push_back(TexMapData{ TexType::Albedo,		"Textures/rustediron2.png" });						//Textures/small_metal_debris.jpg
		texMapDatas.push_back(TexMapData{ TexType::Metallic,	"Textures/Metallic/rustediron2.png" });				//Textures/Metallic/small_metal_debris.jpg
		texMapDatas.push_back(TexMapData{TexType::Roughness,	"Textures/Roughness/rustediron2.png"});				//Textures/Roughness/small_metal_debris.jpg
		texMapDatas.push_back(TexMapData{ TexType::Normal,		"Textures/Normal/rustediron2.png" });				//Textures/Normal/small_metal_debris.jpg
		texMapDatas.push_back(TexMapData{ TexType::Parallax,	"Textures/Parallax/rustediron2.png" });				//Textures/Parallax/small_metal_debris.jpg
		texMapDatas.push_back(TexMapData{ TexType::Glow,		"Textures/Glow/rock.jpg" });						//Textures/Glow/small_metal_debris.jpg
		auto modelMatrix = glm::mat4(1.0f);
		auto prevModelMatrix = glm::mat4(1.0f);

		auto texMap = CreateTextureMap(std::move(texMapDatas));

		std::vector<rw_clustering_ptr<Mesh>> mesh = std::vector<rw_clustering_ptr<Mesh>>();
		mesh.push_back(AddToMeshPool(std::move(mesh0)));
		auto mro = Render_Object(std::move(mesh), std::move(texMap), AddToModelMatrixPool(std::move(modelMatrix)), AddToPrevModelMatrixPool(std::move(prevModelMatrix)));
		unrigStuff.emplace_back(std::move(mro));

		mesh0 = CreatePlane();
		texMapDatas = std::vector<TexMapData>();
		texMapDatas.push_back(TexMapData{ TexType::Albedo,		"Textures/brick_floor.png" });						//Textures/brick.jpg
		texMapDatas.push_back(TexMapData{ TexType::Metallic,	"Textures/Metallic/brick_floor.png" });				//Textures/Metallic/brick.jpg
		texMapDatas.push_back(TexMapData{ TexType::Roughness,	"Textures/Roughness/brick_floor.png" });			//Textures/Roughness/brick.jpg
		texMapDatas.push_back(TexMapData{ TexType::Normal,		"Textures/Normal/brick_floor.png" });				//Textures/Normal/brick.jpg
		texMapDatas.push_back(TexMapData{ TexType::Parallax,	"Textures/Parallax/brick_floor.png" });				//Textures/Parallax/brick.jpg
		texMapDatas.push_back(TexMapData{ TexType::Glow,		"Textures/Glow/brick_floor.png" });					//Textures/Glow/brick.jpg
		modelMatrix = glm::mat4(1.0f);
		prevModelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -4.0f, 0.0f));

		texMap = CreateTextureMap(std::move(texMapDatas));
		
		mesh = std::vector<rw_clustering_ptr<Mesh>>();
		mesh.push_back(AddToMeshPool(std::move(mesh0)));
		mro = Render_Object(std::move(mesh), std::move(texMap), AddToModelMatrixPool(std::move(modelMatrix)), AddToPrevModelMatrixPool(std::move(prevModelMatrix)));
		unrigStuff.push_back(std::move(mro));

		sceneObjRO.push_back(std::move(unrigStuff));

		auto billboardStuff = std::vector<Render_Object>();

		mesh0 = CreateBillboard();
		texMapDatas = std::vector<TexMapData>();
		texMapDatas.push_back(TexMapData{ TexType::Default,		"Textures/grass.png" });
		modelMatrix = glm::mat4(1.0f);
		prevModelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -3.5f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f, 2.0f, 0.0f));

		texMap = CreateTextureMap(std::move(texMapDatas));
		
		mesh = std::vector<rw_clustering_ptr<Mesh>>();
		mesh.push_back(AddToMeshPool(std::move(mesh0)));
		mro = Render_Object(std::move(mesh), std::move(texMap), AddToModelMatrixPool(std::move(modelMatrix)), AddToPrevModelMatrixPool(std::move(prevModelMatrix)));
		billboardStuff.push_back(std::move(mro));

		billboardRO.push_back(std::move(billboardStuff));
	}

	void CreateShaders() {

		buildAABBGridCompShader		= std::make_shared<Shader_Object>(std::vector<std::string>{"Shaders/clusterShader.comp"});
		visibleClusterCompShader	= std::make_shared<Shader_Object>(std::vector<std::string>{"Shaders/clusterVisibleShader.comp"});
		cullLightsCompShader		= std::make_shared<Shader_Object>(std::vector<std::string>{"Shaders/clusterCullLightShader.comp"});
		
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

		//ToDo: #20 simulation manager class
		//fluidFragShader->CreateFromFiles("Shaders/framebuffer.vert", "Shaders/2DFluid/fluid.frag");
		//fluidFragShader3D->CreateFromFiles("Shaders/3DFluid/fluid.vert", "Shaders/3DFluid/fluid.frag");
	}

	//ToDo: Fix and refactor in #33
	//void RenderParticlesScene(GLfloat deltaTime)
	//{
	//	particleList[0]->GenerateParticlesCPU(deltaTime, glm::vec3(10.0f, 33.0f, 0.0f));
	//	particleList[0]->SimulateParticlesCPU(camera->getCameraPosition(), deltaTime);
	//	particleList[0]->UpdateParticlesMeshCPU();
	//	plainTexture->UseTexture(0);
	//	particleList[0]->RenderInstancedMesh();
	//}

	void DirectionalShadowMapPass(DirectionalLight* light, const CamParam& camParam, const glm::ivec2& screenDims) {

		auto testLitView = light->CalculateCascadeLightTransform();
		light->CalcOrthProjs(camParam.View, &testLitView, 60.0f, screenDims);

		glm::mat4 proj[NUM_CASCADES];
		glm::mat4 vView[NUM_CASCADES];

		for (auto i = 0; i < NUM_CASCADES; ++i)
		{
			vView[i] = glm::lookAt(light->GetModlCent(i), light->GetModlCent(i) + glm::normalize(-light->direction) * 0.2f, light->up);
			proj[i] = light->GetProjMat(vView[i], i);
		}

		auto lightParam = LightParam{ nullptr, proj, vView };
		dirShadowRPHandler->Update(sceneObjRO, &camParam, &lightParam);
	}

	void OmniShadowMapPass(const std::vector<std::shared_ptr<PointLight>>& lights, const CamParam& camParam) 
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
		omniShadowRPHandler->Update(sceneObjRO, &camParam, &lightParam);
	}

	void CullLight(const CamParam& camParam, const glm::ivec2& screenDims)
	{
		visibleClusterCompShader->UseShaderObject();
		depthMap->AttachFBOToTextureUnit(0, 0, 0, 0);
		visibleClusterCompShader->DispatchShaderObject(glm::uvec3(screenDims.x / 32, screenDims.y / 30, 1));

		//unsigned int count = 0;
		//unsigned int isvisible[32 * 32 * 10];

		//glGetNamedBufferSubData(visibleClusterSSBO, 0, numClusters * sizeof(unsigned int), &isvisible);
		//for(int i = 0; i < numClusters; ++i)
		//{
		//	if (isvisible[i]) ++count;
		//}

		//4-Light assignment
		cullLightsCompShader->UseShaderObject();
		cullLightsCompShader->SetVariable("View", camParam.View);
		cullLightsCompShader->DispatchShaderObject(glm::uvec3(1, 1, gridSizeZ));

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
		preZRPHandler->Update(sceneObjRO, &camParam);
	}

	void SSAOPass(const CamParam& camParam)
	{
		ssaoRPHandler->Update(ssaoQuadRO, &camParam);
	}

	void SSAOBlurPass(const CamParam& camParam)
	{
		ssaoBlurRPHandler->Update(quadRO, &camParam);
	}

	struct LightData
	{
		std::vector<glm::vec3> positions;
		std::vector<glm::mat4> vView;
		std::vector<glm::vec3> directions;
		std::vector<glm::mat4> projections;
		std::vector<GLfloat> farplanes;
		std::vector<GLfloat> edges;
		std::vector<glm::vec3> colors;
	};

	void RenderPass(const CamParam& camParam, DirectionalLight* dirlight, std::shared_ptr<PointLight>* pointLights, std::shared_ptr<SpotLight>* spotLights)
	{
		std::vector<LightData> lightDataList(pointLightCount + spotLightCount, LightData());
		std::vector<LightParam> lightParamList;

		auto lightData = &lightDataList[0];
		lightData->directions.push_back(dirlight->direction);
		lightData->colors.push_back(dirlight->color);

		for (auto i = 0; i < NUM_CASCADES; ++i)
		{
			lightData->vView.push_back(glm::lookAt(dirlight->GetModlCent(i), dirlight->GetModlCent(i) + glm::normalize(-lightData->directions[0]) * 0.2f, dirlight->up));
			lightData->projections.push_back(dirlight->GetProjMat(lightData->vView[i], i));

			glm::vec4 vViewTemp(0.0f, 0.0f, dirlight->GetCascadeEnd(i + 1), 1.0f);
			auto vClip = camParam.Projection * vViewTemp;
			//printf("%F \n", vClip.z);
			lightData->edges.push_back(-vClip.z);
		}

		auto lightParam = LightParam{ nullptr, &lightData->projections[0], &lightData->vView[0], &lightData->directions[0], nullptr, &lightData->edges[0], 1, &lightData->colors[0]};
		lightParamList.push_back(std::move(lightParam));

		lightData = &lightDataList[1];

		for (auto i = 0; i < pointLightCount; ++i)
		{
			lightData->farplanes.push_back(pointLights[i]->farPlane);
		}

		lightParam = LightParam(nullptr, nullptr, nullptr, nullptr, &lightData->farplanes[0], nullptr, pointLightCount, nullptr);
		lightParamList.push_back(std::move(lightParam));

		lightData = &lightDataList[2];

		for (auto i = 0; i < spotLightCount; ++i)
		{
			lightData->positions.push_back(spotLights[i]->position);
			lightData->directions.push_back(spotLights[i]->direction);
			lightData->projections.push_back(spotLights[i]->lightProj);
			lightData->farplanes.push_back(spotLights[i]->farPlane);
			lightData->edges.push_back(spotLights[i]->procEdge);
			lightData->colors.push_back(spotLights[i]->color);
		}

		lightParam = LightParam(&lightData->positions[0], &lightData->projections[0], nullptr, &lightData->directions[0], &lightData->farplanes[0], &lightData->edges[0], spotLightCount, &lightData->colors[0]);
		lightParamList.push_back(std::move(lightParam));
		sceneRPHandler->Update(sceneObjRO, &camParam, &lightParamList[0]);
		//Note: **Important** skybox being after transparent mesh causes blending issues
		skyboxRPHandler->Update(cwCubeRO, &camParam);		
		billBoardRPHandler->Update(billboardRO, &camParam);
	}

	void Bloom()
	{
		bloomRPHandler->Update(quadRO);
	}

	void MotionBlurPass(const CamParam& camParam)
	{
		motionBlurRPHandler->Update(quadRO, &camParam);
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

	~Impl() noexcept
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

RenderEngineMain::RenderEngineMain(const glm::ivec2& screenDims) : m_pImpl{ std::make_unique<Impl>() } { Pimpl()->Init(screenDims); };

void RenderEngineMain::Update(const glm::ivec2& screenDims, const bool& isUpdateBuffers)
{	
	Pimpl()->Update(screenDims, isUpdateBuffers);
}

void RenderEngineMain::EndUpdate() 
{
	Pimpl()->EndUpdate();
}

const GLuint RenderEngineMain::GetFboBuffer(const std::string& fboHandlerName, const GLuint& fboIndex, const GLuint& bufferIndex) const
{
	auto fboHndlr = Pimpl()->m_SceneFboHandlerMgr->FindFboHandler(fboHandlerName);
	return fboHndlr != nullptr ? fboHndlr->GetFBOBuffer(fboIndex, bufferIndex) : 0;
}

RenderEngineMain::~RenderEngineMain() = default;