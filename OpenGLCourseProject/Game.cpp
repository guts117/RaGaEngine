#include "Game.h"
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

float terrainScaleFactor = 0.0f;
float terrainScaleFactor1 = 1000.0f;

Game::Game()
{
 //empty
}

void Game::init()
{
	mainWindow.Initialise();
	CreateBillboard();
	CreateParticles();
	CreateTerrain();
	CreateObject();
	CreateShaders();

	camera = std::make_shared<Camera>(glm::vec3(-terrainScaleFactor, 30.0f, -terrainScaleFactor), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 50.0f, 0.2f);

	environmentTexture = std::make_unique<Texture>("Textures/HDR/GCanyon_C_YumaPoint_3k.hdr");
	environmentTexture->LoadTextureHDR();

	brickTexture = std::make_unique <Texture>("Textures/brick.jpg");
	brickTexture->LoadTextureSRGB();
	metalDebrisTexture = std::make_unique <Texture>("Textures/small_metal_debris.jpg");
	metalDebrisTexture->LoadTextureSRGB();
	floorTexture = std::make_unique <Texture>("Textures/brick_floor.png");
	floorTexture->LoadTextureSRGB();
	rustedMetal = std::make_unique <Texture>("Textures/rustediron2.png");
	rustedMetal->LoadTextureSRGBA();

	plainTexture = std::make_unique <Texture>("Textures/plain.png");
	plainTexture->LoadTextureSRGB();
	
	grassTexture = std::make_unique <Texture>("Textures/grass.png");
	grassTexture->LoadTextureSRGBA();

	brickTextureMetal = std::make_unique <Texture>("Textures/Metallic/brick.jpg");
	brickTextureMetal->LoadTexture();
	metalDebrisTextureMetal = std::make_unique <Texture>("Textures/Metallic/small_metal_debris.jpg");
	metalDebrisTextureMetal->LoadTexture();
	floorTextureMetal = std::make_unique <Texture>("Textures/Metallic/brick_floor.png");
	floorTextureMetal->LoadTexture();
	rustedMetalMetal = std::make_unique <Texture>("Textures/Metallic/rustediron2.png");
	rustedMetalMetal->LoadTexture();

	brickTextureNorm = std::make_unique <Texture>("Textures/Normal/brick.jpg");
	brickTextureNorm->LoadTexture();
	metalDebrisTextureNorm = std::make_unique <Texture>("Textures/Normal/small_metal_debris.jpg");
	metalDebrisTextureNorm->LoadTexture();
	floorTextureNorm = std::make_unique <Texture>("Textures/Normal/brick_floor.png");
	floorTextureNorm->LoadTexture();
	rustedMetalNorm = std::make_unique <Texture>("Textures/Normal/rustediron2.png");
	rustedMetalNorm->LoadTexture();

	brickTextureRough = std::make_unique <Texture>("Textures/Roughness/brick.jpg");
	brickTextureRough->LoadTexture();
	metalDebrisTextureRough = std::make_unique <Texture>("Textures/Roughness/small_metal_debris.jpg");
	metalDebrisTextureRough->LoadTexture();
	floorTextureRough = std::make_unique <Texture>("Textures/Roughness/brick_floor.png");
	floorTextureRough->LoadTexture();
	rustedMetalRough = std::make_unique <Texture>("Textures/Roughness/rustediron2.png");
	rustedMetalRough->LoadTexture();

	brickTexturePara = std::make_unique <Texture>("Textures/Parallax/brick.jpg");
	brickTexturePara->LoadTexture();
	metalDebrisTexturePara = std::make_unique <Texture>("Textures/Parallax/small_metal_debris.jpg");
	metalDebrisTexturePara->LoadTexture();
	floorTexturePara = std::make_unique <Texture>("Textures/Parallax/brick_floor.png");
	floorTexturePara->LoadTexture();
	rustedMetalPara = std::make_unique <Texture>("Textures/Parallax/rustediron2.png");
	rustedMetalPara->LoadTexture();

	brickTextureGlow = std::make_unique <Texture>("Textures/Glow/brick.jpg");
	brickTextureGlow->LoadTexture();
	metalDebrisTextureGlow = std::make_unique <Texture>("Textures/Glow/small_metal_debris.jpg");
	metalDebrisTextureGlow->LoadTexture();
	floorTextureGlow = std::make_unique <Texture>("Textures/Glow/brick_floor.png");
	floorTextureGlow->LoadTexture();
	rustedMetalGlow = std::make_unique <Texture>("Textures/Glow/rock.jpg");
	rustedMetalGlow->LoadTexture();

	terrainTextureDisp = std::make_unique <Texture>("Textures/Displacement/terrain.jpg");
	terrainTextureDisp->LoadTexture();
	terrainTextureBlend = std::make_unique <Texture>("Textures/Blend/terrain.jpg");
	terrainTextureBlend->LoadTexture();
	terrainTexture = std::make_unique <Texture>("Textures/terrain");
	terrainTexture->LoadTextureArray(true, false);
	terrainTextureMetal = std::make_unique <Texture>("Textures/Metallic/terrain");
	terrainTextureMetal->LoadTextureArray(false, true);
	terrainTextureRough = std::make_unique <Texture>("Textures/Roughness/terrain");
	terrainTextureRough->LoadTextureArray(false, true);
	terrainTextureNorm = std::make_unique <Texture>("Textures/Normal/terrain");
	terrainTextureNorm->LoadTextureArray(false, true);
	terrainTexturePara = std::make_unique <Texture>("Textures/Parallax/terrain");
	terrainTexturePara->LoadTextureArray(false, true);


	shinyMaterialGlow = std::make_shared<Material>(1, 6, 7, 11, 12, 13);
	dullMaterialGlow = std::make_unique<Material>(1, 6, 7, 11, 12, 13);

	shinyMaterialPara = std::make_unique<Material>(1, 6, 7, 11,12);
	dullMaterialPara = std::make_unique<Material>(1, 6, 7, 11,12);

	shinyMaterialRough= std::make_unique<Material>(1, 6, 7, 11);
	dullMaterialRough = std::make_unique<Material>(1, 6, 7, 11);

	shinyMaterialNorm = std::make_unique<Material>(1, 6, 7);
	dullMaterialNorm = std::make_unique<Material>(1, 6, 7);

	shinyMaterialMetal = std::make_unique<Material>(1, 6);
	dullMaterialMetal = std::make_unique<Material>(1, 6);

	shinyMaterial = std::make_unique<Material>(1, 1);
	dullMaterial = std::make_unique<Material>(1, 1);
	
	shinyTerrainMaterial = std::make_unique<Material>(12, 13, 15, 16, 17);
	dullTerrainMaterial = std::make_unique<Material>(12, 13, 15, 16, 17);
	
	cube->LoadModel("Models/cube.obj");

	sphere->LoadModel("Models/sphere.obj");
	
	sniper->LoadModel("Models/Sniper_rifle_KSR-29.fbx");

	gun->LoadModel("Models/Cerberus_LP.fbx");

	anymodel->LoadModel("Models/Intergalactic_Spaceship-(Wavefront).obj");

	bulb->LoadModel("Models/Free_Antique_Bulb.obj");

	anim->LoadModel("Models/boblampclean.md5mesh");

	anim2->LoadModel("Models/model.dae");

	environmentMap = std::make_unique<Equirectangular_to_CubeMap_Framebuffer>();
	environmentMap->Init(ScreenWidth, ScreenWidth, true);

	irradianceMap = std::make_unique<Equirectangular_to_CubeMap_Framebuffer>();
	irradianceMap->Init(32, 32, false);

	prefilterMap = std::make_unique <PreFilter_Framebuffer>();
	prefilterMap->Init(128, 128);

	brdfMap = std::make_unique < BRDF_Framebuffer>();
	brdfMap->Init(ScreenWidth, ScreenWidth);

	quad = std::make_unique < Static_Mesh>();
	mesh_cube = std::make_unique < Static_Mesh>();

	depth = std::make_unique < Depth_Framebuffer>();
	depth->Init(ScreenWidth, ScreenHeight);

	ssao = std::make_unique < SSAO_Framebuffer>();
	ssao->Init(ScreenWidth, ScreenHeight);

	ssaoBlur = std::make_unique < SSAOBlur_Framebuffer>();
	ssaoBlur->Init(ScreenWidth, ScreenHeight);

	hdr = std::make_unique < HDR_Framebuffer>();
	hdr->Init(ScreenWidth, ScreenHeight);

	blur = std::make_unique < Blur_PingPong_Framebuffer>();
	blur->Init(ScreenWidth, ScreenHeight);

	motionBlur = std::make_unique < Motion_Blur_FrameBuffer>();
	motionBlur->Init(ScreenWidth, ScreenHeight);

	mainLight = std::make_shared < DirectionalLight>(2048, 2048,
		3.0f, 3.0f, 3.0f,
		5500.0f, -5500.0f, -10000.0f);

	pointLights[0] = std::make_shared < PointLight>(1024, 1024,
		0.1f, 100.0f,
		0.1f, 0.1f, 0.1f,
		6.0f - terrainScaleFactor, 40.0f, 10.0f - terrainScaleFactor);

	pointLightCount++;

	/*pointLights[1] = PointLight(1024, 1024,
		0.1f, 100.0f,
		0.0f, 1.0f, 0.0f,
		-3.0f, 4.0f, 0.0f);

	pointLightCount++;*/

	spotLights[0] = std::make_shared < SpotLight>(1024, 1024,
		0.1f, 100.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		10.0f);

	spotLightCount++;

	//std::vector<std::string> skyboxFaces;
	///*skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	//skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	//skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	//skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	//skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	//skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");*/
	//skyboxFaces.push_back("Textures/Skybox/barren_rt.jpg");
	//skyboxFaces.push_back("Textures/Skybox/barren_lf.jpg");
	//skyboxFaces.push_back("Textures/Skybox/barren_up.jpg");
	//skyboxFaces.push_back("Textures/Skybox/barren_dn.jpg");
	//skyboxFaces.push_back("Textures/Skybox/barren_bk.jpg");
	//skyboxFaces.push_back("Textures/Skybox/barren_ft.jpg");


	skybox = std::make_unique<Skybox>();

	//skyboxTexture.LoadCubeMapSRGB(skyboxFaces);

	projection = glm::perspective(glm::radians(60.0f), (GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(), 0.1f, 10000.0f);


	//SSAO initialization
	ssaoShader->UseShader();

	uniformSampleRadius = ssaoShader->GetSampleRadiusLocation();
	uniformProjectionAO = ssaoShader->GetProjectionLocation();

	glUniformMatrix4fv(uniformProjectionAO, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform1f(uniformSampleRadius, 0.1f);

	ssaoShader->GenKernel();
	ssaoShader->GenNoise(ssaoNoiseData);

	SSAONoiseTexture->GenerateNoiseTexture(ssaoNoiseData);

	terrainShader->UseShader();
	for (size_t i = 0; i < NUM_CASCADES; ++i)
	{
		glm::vec4 vView(0.0f, 0.0f, mainLight->GetShadowMap()->GetCascadeEnd(i+1), 1.0f);
		glm::vec4 vClip = projection * vView;
		printf("%F \n", vClip.z);
		terrainShader->SetCascadeEndClipSpace(i, -vClip.z);
	}

	EnvironmentMapPass();
	IrradianceConvolutionPass();
	PrefilterPass();
	BRDFPass();

	Bulb = std::make_unique<Static_Object>();
	Bulb->SetUpImportedModelData("Models/Free_Antique_Bulb.obj");
}

void Game::update(float fps) {
	GLfloat now = static_cast<GLfloat>(glfwGetTime()); //SDL_GetPerformanceCounter();
	deltaTime = now - lastTime;	//(now-lastTime)*1000/SDL_GetPerfomnaceFrequency(); //for seconds
	lastTime = now;

	//get + handle user input events
	glfwPollEvents();
	camera->keyControl(mainWindow.getKeys(), deltaTime);
	camera->mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

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

	if (mainWindow.getKeys()[GLFW_KEY_L]) {
		spotLights[0]->Toggle();
		mainWindow.getKeys()[GLFW_KEY_L] = false;
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DirectionalShadowMapPass(camera->calculateViewMatrix(), mainLight.get());

	for (size_t i = 0; i < pointLightCount; i++) {
		OmniShadowMapPass(pointLights[i].get());
	}
	for (size_t i = 0; i < spotLightCount; i++) {
		OmniShadowMapPass(spotLights[i].get());
	}

	PreZPass(projection, camera->calculateViewMatrix(), deltaTime);
	SSAOPass(projection);
	SSAOBlurPass();
	RenderPass(projection, camera->calculateViewMatrix(), deltaTime);
	BlurPass();
	MotionBlurPass(fps);
	BloomPass();

	prevProjView = projection *camera->calculateViewMatrix();
	prevProj = projection;
	prevView = camera->calculateViewMatrix();
	glUseProgram(0);

	mainWindow.swapBuffers();
}

void Game::calcAverageNormals(unsigned int* indices, unsigned int indicesCount,
	GLfloat* vertices, unsigned int verticesCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indicesCount; i += 3) {
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		
		vertices[in0] += -normal.x; vertices[in0 + 1] += -normal.y; vertices[in0 + 2] += -normal.z;
		vertices[in1] += -normal.x; vertices[in1 + 1] += -normal.y; vertices[in1 + 2] += -normal.z;
		vertices[in2] += -normal.x; vertices[in2 + 1] = -normal.y; vertices[in2 + 2] = -normal.z;

	}

	for (size_t i = 0; i < verticesCount / vLength; i++) {
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void Game::calcAverageTangents(unsigned int* indices, unsigned int indicesCount,
	GLfloat* vertices, unsigned int verticesCount,
	unsigned int vLength, unsigned int tangentOffset)
{
	for (size_t i = 0; i < indicesCount; i += 3) {
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;

		glm::vec3 Edge1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 Edge2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);

		float DeltaU1 = vertices[in1 + 3] - vertices[in0 + 3];                //for the uv coordinates add 3
		float DeltaV1 = vertices[in1 + 3 + 1] - vertices[in0 + 3 + 1];
		float DeltaU2 = vertices[in2 + 3] - vertices[in0 + 3];
		float DeltaV2 = vertices[in2 + 3 + 1] - vertices[in0 + 3 + 1];

		float f = 1.0f / (DeltaU1 * DeltaV2 - DeltaU2 * DeltaV1);

		glm::vec3 Tangent, Bitangent;

		Tangent.x = f * (DeltaV2 * Edge1.x - DeltaV1 * Edge2.x);
		Tangent.y = f * (DeltaV2 * Edge1.y - DeltaV1 * Edge2.y);
		Tangent.z = f * (DeltaV2 * Edge1.z - DeltaV1 * Edge2.z);

		/*Bitangent.x = f * (-DeltaU2 * Edge1.x - DeltaU1 * Edge2.x);
		Bitangent.y = f * (-DeltaU2 * Edge1.y - DeltaU1 * Edge2.y);
		Bitangent.z = f * (-DeltaU2 * Edge1.z - DeltaU1 * Edge2.z);*/

		in0 += tangentOffset; in1 += tangentOffset; in2 += tangentOffset;
		vertices[in0] += -Tangent.x; vertices[in0 + 1] += -Tangent.y; vertices[in0 + 2] += -Tangent.z;
		vertices[in1] += -Tangent.x; vertices[in1 + 1] += -Tangent.y; vertices[in1 + 2] += -Tangent.z;
		vertices[in2] += -Tangent.x; vertices[in2 + 1] += -Tangent.y; vertices[in2 + 2] += -Tangent.z;
	}

	for (size_t i = 0; i < verticesCount / vLength; i++) {
		unsigned int nOffset = i * vLength + tangentOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void Game::CreateBillboard() {

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

	std::shared_ptr<Static_Mesh> obj = std::make_shared <Static_Mesh>();
	obj->CreateBillboard(billboardVertices, billboardIndices, 12, 6);
	billboardList.push_back(obj);

}

void Game::CreateParticles()
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

	obj->CreateParticlesMeshCPU(particlesVertices, particlesIndices, 12, 6);
	particleList.push_back(obj);
}

void Game::CreateTerrain()
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

	calcAverageNormals(terrainIndices, 6, terrainVertices, 44, 11, 5);
	calcAverageTangents(terrainIndices, 6, terrainVertices, 44, 11, 8);

	std::shared_ptr < Static_Mesh> obj = std::make_shared< Static_Mesh>();
	obj->CreateMeshNorm(terrainVertices, terrainIndices, 44, 6);
	terrainList.push_back(obj);
}

void Game::CreateObject() {
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


	calcAverageNormals(indices, 12, vertices, 44, 11, 5);
	calcAverageTangents(indices, 12, vertices, 44, 11, 8);
	calcAverageNormals(floorIndices, 6, floorVertices, 44, 11, 5);
	calcAverageTangents(floorIndices, 6, floorVertices, 44, 11, 8);

	std::shared_ptr < Static_Mesh> obj1 = std::make_shared <Static_Mesh>();
	obj1->CreateMeshNorm(vertices, indices, 44, 12);
	meshList.push_back(obj1);

	std::shared_ptr < Static_Mesh> obj2 = std::make_shared <Static_Mesh>();
	obj2->CreateMeshNorm(vertices, indices, 44, 12);
	meshList.push_back(obj2);

	std::shared_ptr < Static_Mesh> obj3 = std::make_shared <Static_Mesh>();
	obj3->CreateMeshNorm(floorVertices, floorIndices, 44, 6);
	meshList.push_back(obj3);

	std::shared_ptr < Static_Mesh> obj4 = std::make_shared <Static_Mesh>();
	obj4->CreateMeshNorm(floorVertices, floorIndices, 44, 6);
	meshList.push_back(obj4);
}

void Game::CreateShaders() {

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
	shader1->CreateFromFiles(vShader.c_str(), fShader.c_str());
	shaderList.push_back(shader1);

	std::shared_ptr<Model_Shader> shader2 = std::make_shared < Model_Shader>();
	shader2->CreateFromFiles(avShader.c_str(), fShader.c_str());
	animShaderList.push_back(shader2);

	terrainShader->CreateFromFiles("Shaders/terrain.vert", "Shaders/terrain.tessc", "Shaders/terrain.tesse", "Shaders/terrain.frag");
	billboardShader->CreateFromFiles("Shaders/billboard.vert", "Shaders/billboard.frag");

	particleShader->CreateFromFiles("Shaders/particles.vert", "Shaders/particles.frag");

	hdrShader->CreateFromFiles("Shaders/framebuffer.vert", "Shaders/hdr_framebuffer.frag");

	motionBlurShader->CreateFromFiles("Shaders/framebuffer.vert", "Shaders/motionBlur_framebuffer.frag");

	blurShader->CreateFromFiles("Shaders/framebuffer.vert", "Shaders/blur_framebuffer.frag");

	shader1 = nullptr;
	shader2 = nullptr;
}

void Game::RenderBillboardScene()
{
	glm::mat4 prevPV = glm::mat4();
	glUniform3f(uniformPos, 6.0f - terrainScaleFactor, 29.0f, -terrainScaleFactor);
	glUniform2f(uniformSize, 2.0f, 2.0f/*0.125f*/);

	prevPV = prevProjView;
	glUniformMatrix4fv(uniformPrevPV0, 1, GL_FALSE, glm::value_ptr(prevPV));

	grassTexture->UseTexture(0);
	billboardList[0]->RenderMesh();
}

void Game::RenderParticlesScene(GLfloat deltaTime)
{
	particleList[0]->GenerateParticlesCPU(deltaTime, glm::vec3(10.0f - terrainScaleFactor, 33.0f, -terrainScaleFactor));
	particleList[0]->SimulateParticlesCPU(camera->getCameraPosition(), deltaTime);
	particleList[0]->UpdateParticlesMeshCPU();
	plainTexture->UseTexture(0);
	particleList[0]->RenderParticlesMeshCPU();
}

void Game::RenderTerrain(bool shadow, bool depth)
{
	glUniform1f(uniformDispFactor,(0.2f*terrainScaleFactor1));

	glm::mat4 model;
	glm::mat4 prevPVM = glm::mat4();
	//model = glm::translate(model, glm::vec3(0.0f,-10.0f, 0.0f));
	//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
	//model = glm::scale(model,glm::vec3(terrainScaleFactor, 1.0f, terrainScaleFactor));
	glUniformMatrix4fv(uniformModel2, 1, GL_FALSE, glm::value_ptr(model));
	prevPVM = prevProjView * terrainList[0]->prevMesh;
	glUniformMatrix4fv(uniformPrevPVM2, 1, GL_FALSE, glm::value_ptr(prevPVM));
	terrainTextureDisp->UseTexture(0);
	terrainTextureBlend->UseTexture(10);
	if(shadow)
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
	terrainList[0]->prevMesh = model;	
}

void Game::RenderEnvCubeMap(bool is_cubeMap)
{
	if(is_cubeMap)
	{
		environmentMap->Read(GL_TEXTURE1);
	}
	else
	{
		environmentTexture->UseTexture(0);
	}
	mesh_cube->RenderCube();
}

void Game::RenderScene(std::shared_ptr<Shader> shader, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
	glm::mat4 model = glm::mat4();;
	glm::mat4 prevPVM = glm::mat4();

	model = glm::translate(model, glm::vec3(-terrainScaleFactor, 34.0f, -2.5f - terrainScaleFactor));
	/*model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));*/
	//model = glm::scale(model,glm::vec3(0.4f,0.4f,1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	prevPVM = prevProjView * meshList[0]->prevMesh;
	glUniformMatrix4fv(uniformPrevPVM, 1, GL_FALSE, glm::value_ptr(prevPVM));
	rustedMetal->UseTexture(0);
	rustedMetalMetal->UseTexture(5);
	rustedMetalNorm->UseTexture(6);
	rustedMetalRough->UseTexture(10);
	rustedMetalPara->UseTexture(11);
	rustedMetalGlow->UseTexture(12);
	shinyMaterialGlow->UseMaterial(uniformAlbedoMap, uniformMetallicMap, uniformNormalMap, uniformRoughnessMap, uniformParallaxMap, uniformGlowMap);
	meshList[0]->RenderMesh();
	meshList[0]->prevMesh = model;
	model = glm::mat4();

	//model = glm::rotate(model, -aircraftAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(curScale - terrainScaleFactor, 32.0f, 4.5f - terrainScaleFactor));
	//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
	model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, 90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, 180.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -20.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, toRadians, glm::vec3(0.0f, 1.0f, 0.0f));  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	prevPVM = prevProjView*cube->prevModel;
	glUniformMatrix4fv(uniformPrevPVM, 1, GL_FALSE, glm::value_ptr(prevPVM));
	shinyMaterialGlow->UseMaterial(uniformAlbedoMap, uniformMetallicMap, uniformNormalMap, uniformRoughnessMap, uniformParallaxMap, uniformGlowMap);
	cube->RenderModel();
	cube->prevModel = model;
	model = glm::mat4();

	model = glm::translate(model, glm::vec3(-terrainScaleFactor, 35.0f, 5.5f - terrainScaleFactor));
	model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	prevPVM = prevProjView * sphere->prevModel;
	glUniformMatrix4fv(uniformPrevPVM, 1, GL_FALSE, glm::value_ptr(prevPVM));
	shinyMaterialGlow->UseMaterial(uniformAlbedoMap, uniformMetallicMap, uniformNormalMap, uniformRoughnessMap, uniformParallaxMap, uniformGlowMap);
	sphere->RenderModel();
	sphere->prevModel = model;	
	model = glm::mat4();

	model = glm::translate(model, glm::vec3(-terrainScaleFactor, 30.0f, -2.5f - terrainScaleFactor));
	//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
	//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	prevPVM = prevProjView * meshList[1]->prevMesh;
	glUniformMatrix4fv(uniformPrevPVM, 1, GL_FALSE, glm::value_ptr(prevPVM));
	metalDebrisTexture->UseTexture(0);
	metalDebrisTextureMetal->UseTexture(5);
	metalDebrisTextureNorm->UseTexture(6);
	metalDebrisTextureRough->UseTexture(10);
	metalDebrisTexturePara->UseTexture(11);
	metalDebrisTextureGlow->UseTexture(12);
	dullMaterialGlow->UseMaterial(uniformAlbedoMap, uniformMetallicMap, uniformNormalMap, uniformRoughnessMap, uniformParallaxMap, uniformGlowMap);
	meshList[1]->RenderMesh();
	meshList[1]->prevMesh = model;
	model = glm::mat4();

	model = glm::translate(model, glm::vec3(-15.0f - terrainScaleFactor, 43.0f, -terrainScaleFactor));
	model = glm::rotate(model, 90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
	//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	prevPVM = prevProjView * meshList[2]->prevMesh;
	glUniformMatrix4fv(uniformPrevPVM, 1, GL_FALSE, glm::value_ptr(prevPVM));
	floorTexture->UseTexture(0);
	floorTextureMetal->UseTexture(5);
	floorTextureNorm->UseTexture(6);
	floorTextureRough->UseTexture(10);
	floorTexturePara->UseTexture(11);
	floorTextureGlow->UseTexture(12);
	shinyMaterialGlow->UseMaterial(uniformAlbedoMap, uniformMetallicMap, uniformNormalMap, uniformRoughnessMap, uniformParallaxMap, uniformGlowMap);
	meshList[2]->RenderMesh();
	meshList[2]->prevMesh = model;
	model = glm::mat4();

	model = glm::translate(model, glm::vec3(-terrainScaleFactor, 43.0f, -15.0f - terrainScaleFactor));
	model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	prevPVM = prevProjView * meshList[3]->prevMesh;
	glUniformMatrix4fv(uniformPrevPVM, 1, GL_FALSE, glm::value_ptr(prevPVM));
	brickTexture->UseTexture(0);
	brickTextureMetal->UseTexture(5);
	brickTextureNorm->UseTexture(6);
	brickTextureRough->UseTexture(10);
	brickTexturePara->UseTexture(11);
	brickTextureGlow->UseTexture(12);
	shinyMaterialGlow->UseMaterial(uniformAlbedoMap, uniformMetallicMap, uniformNormalMap, uniformRoughnessMap, uniformParallaxMap, uniformGlowMap);
	meshList[3]->RenderMesh();
	meshList[3]->prevMesh = model;

				/*model = glm::mat4();
					model = glm::translate(model, glm::vec3(15.0f, 13.0f, 0.0f));
					model = glm::rotate(model, 90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
					model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
					//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
					//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

					brickTexture.UseTexture(0);
					brickTextureSpec.UseTexture(5);
					brickTextureNorm.UseTexture(6);
					brickTextureRefl.UseTexture(8);
					brickTexturePara.UseTexture(9);
					brickTextureGlow.UseTexture(10);
					shinyMaterialGlow.UseMaterial(uniformSpecularIntensity, uniformShininess, uniformDiffuse, uniformSpecular, uniformNormalMap, uniformReflectMap, uniformParallaxMap, uniformGlowMap);
					meshList[3]->RenderMesh();

					model = glm::mat4();
					model = glm::translate(model, glm::vec3(0.0f, 15.0f+13.0f, 0.0f));
					model = glm::rotate(model, 180.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
					//model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
					//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
					//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

					brickTexture.UseTexture(0);
					brickTextureSpec.UseTexture(5);
					brickTextureNorm.UseTexture(6);
					brickTextureRefl.UseTexture(8);
					brickTexturePara.UseTexture(9);
					brickTextureGlow.UseTexture(10);
					shinyMaterialGlow.UseMaterial(uniformSpecularIntensity, uniformShininess, uniformDiffuse, uniformSpecular, uniformNormalMap, uniformReflectMap, uniformParallaxMap, uniformGlowMap);
					meshList[3]->RenderMesh();

					model = glm::mat4();
					model = glm::translate(model, glm::vec3(0.0f, 13.0f, 15.0f));
					model = glm::rotate(model, 90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
					model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
					//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
					//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

					brickTexture.UseTexture(0);
					brickTextureSpec.UseTexture(5);
					brickTextureNorm.UseTexture(6);
					brickTextureRefl.UseTexture(8);
					brickTexturePara.UseTexture(9);
					brickTextureGlow.UseTexture(10);
					shinyMaterialGlow.UseMaterial(uniformSpecularIntensity, uniformShininess, uniformDiffuse, uniformSpecular, uniformNormalMap, uniformReflectMap, uniformParallaxMap, uniformGlowMap);
					meshList[3]->RenderMesh();

					model = glm::mat4();
					model = glm::translate(model, glm::vec3(0.0f, 13.0f, -15.0f));
					model = glm::rotate(model, 90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
					//model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
					//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
					//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

					brickTexture.UseTexture(0);
					brickTextureSpec.UseTexture(5);
					brickTextureNorm.UseTexture(6);
					brickTextureRefl.UseTexture(8);
					brickTexturePara.UseTexture(9);
					brickTextureGlow.UseTexture(10);
					shinyMaterialGlow.UseMaterial(uniformSpecularIntensity, uniformShininess, uniformDiffuse, uniformSpecular, uniformNormalMap, uniformReflectMap, uniformParallaxMap, uniformGlowMap);
					meshList[3]->RenderMesh();*/

	//aircraftAngle += 0.05f;
	//if (aircraftAngle > 360.0f) {
	//	aircraftAngle = 0.05f;
	//}

	model = glm::mat4();
	//model = glm::rotate(model, -aircraftAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(-terrainScaleFactor, 33.0f, 10.0f - terrainScaleFactor));
	model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
	model = glm::rotate(model, 90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, 180.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -20.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, toRadians, glm::vec3(0.0f, 1.0f, 0.0f));  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	prevPVM = prevProjView * sniper->prevModel;
	glUniformMatrix4fv(uniformPrevPVM, 1, GL_FALSE, glm::value_ptr(prevPVM));
	shinyMaterialGlow->UseMaterial(uniformAlbedoMap, uniformMetallicMap, uniformNormalMap, uniformRoughnessMap, uniformParallaxMap, uniformGlowMap);
	sniper->RenderModel();
	sniper->prevModel = model;
	model = glm::mat4();

	//model = glm::rotate(model, -aircraftAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(5.0f-terrainScaleFactor, 33.0f, 10.0f - terrainScaleFactor));
	model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
	model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	prevPVM = prevProjView * gun->prevModel;
	glUniformMatrix4fv(uniformPrevPVM, 1, GL_FALSE, glm::value_ptr(prevPVM));
	shinyMaterialGlow->UseMaterial(uniformAlbedoMap, uniformMetallicMap, uniformNormalMap, uniformRoughnessMap, uniformParallaxMap, uniformGlowMap);
	gun->RenderModel();
	gun->prevModel = model;
	model = glm::mat4();

	model = glm::translate(model, glm::vec3(pointLights[0]->GetPosition().x, pointLights[0]->GetPosition().y+1.0f, pointLights[0]->GetPosition().z));
	//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
	//model = glm::rotate(model, 180.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	prevPVM = prevProjView * bulb->prevModel;
	glUniformMatrix4fv(uniformPrevPVM, 1, GL_FALSE, glm::value_ptr(prevPVM));
	shinyMaterialGlow->UseMaterial(uniformAlbedoMap, uniformMetallicMap, uniformNormalMap, uniformRoughnessMap, uniformParallaxMap, uniformGlowMap);
	bulb->RenderModel();
	bulb->prevModel = model;
	model = glm::mat4();

	Bulb->DrawImportedObject(shader, prevProjView, model, glm::vec3(pointLights[0]->GetPosition().x, pointLights[0]->GetPosition().y + 3.0f, pointLights[0]->GetPosition().z), glm::vec3(10.0f, 10.0f, 10.0f));
	model = glm::mat4();

	model = glm::translate(model, glm::vec3(-terrainScaleFactor, 37.0f, 1.0f - terrainScaleFactor));
	//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
	model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	prevPVM = prevProjView * anymodel->prevModel;
	glUniformMatrix4fv(uniformPrevPVM, 1, GL_FALSE, glm::value_ptr(prevPVM));
	shinyMaterialGlow->UseMaterial(uniformAlbedoMap, uniformMetallicMap, uniformNormalMap, uniformRoughnessMap, uniformParallaxMap, uniformGlowMap);
	anymodel->RenderModel();
	anymodel->prevModel = model;
}

void Game::RenderAnimScene(bool shadow, bool depth) {
	glm::mat4 model;
	glm::mat4 prevPVM = glm::mat4();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-6.0f - terrainScaleFactor, 28.2f, -5.0f - terrainScaleFactor));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
	model = glm::scale(model, glm::vec3(0.1, 0.1f, 0.1f));
	glUniformMatrix4fv(uniformModel1, 1, GL_FALSE, glm::value_ptr(model));
	prevPVM = prevProjView * anim->prevModel;
	glUniformMatrix4fv(uniformPrevPVM1, 1, GL_FALSE, glm::value_ptr(prevPVM));
	shinyMaterialGlow->UseMaterial(uniformAlbedoMap1, uniformMetallicMap1, uniformNormalMap1, uniformRoughnessMap1, uniformParallaxMap1, uniformGlowMap1);

	if (shadow)
	{
		anim->initShaders(animDirectionalShadowShader->GetShaderID());
	}
	else if (depth)
	{
		anim->initShaders(anim_preZPassShader->GetShaderID());
	}
	else{
		anim->initShaders(animShaderList[0]->GetShaderID());
	}
	anim->RenderModel();
	anim->prevModel = model;
	
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(6.0f - terrainScaleFactor, 28.2f, -5.0f - terrainScaleFactor));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	glUniformMatrix4fv(uniformModel1, 1, GL_FALSE, glm::value_ptr(model));
	prevPVM = prevProjView * anim2->prevModel;
	glUniformMatrix4fv(uniformPrevPVM1, 1, GL_FALSE, glm::value_ptr(prevPVM));
	shinyMaterialGlow->UseMaterial(uniformAlbedoMap1, uniformMetallicMap1, uniformNormalMap1, uniformRoughnessMap1, uniformParallaxMap1, uniformGlowMap1);

	if (shadow)
	{
		anim2->initShaders(animDirectionalShadowShader->GetShaderID());
	}
	else if (depth)
	{
		anim2->initShaders(anim_preZPassShader->GetShaderID());
	}
	else
	{
		anim2->initShaders(animShaderList[0]->GetShaderID());
	}

	anim2->RenderModel();
	anim2->prevModel = model;
}

void Game::EnvironmentMapPass()
{
	environmentMapShader->UseShader();
	environmentMapShader->SetTexture(1);

	uniformProjectionEnv = environmentMapShader->GetProjectionLocation();
	glUniformMatrix4fv(uniformProjectionEnv, 1, GL_FALSE, glm::value_ptr(captureProjection));

	glViewport(0, 0, environmentMap->GetWidth(), environmentMap->GetHeight());
	environmentMap->Write(-1);
	for (unsigned int i = 0; i < 6; ++i)
	{
		uniformViewEnv = environmentMapShader->GetViewLocation();
		glUniformMatrix4fv(uniformViewEnv, 1, GL_FALSE, glm::value_ptr(captureViews[i]));
		environmentMap->Write(i);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		environmentMapShader->Validate();

		RenderEnvCubeMap(false);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	environmentMap->CreateFirstMipFace();
}

void Game::IrradianceConvolutionPass()
{
	irradianceConvolutionShader->UseShader();
	irradianceConvolutionShader->SetSkybox(1);

	uniformProjectionIrr = irradianceConvolutionShader->GetProjectionLocation();
	glUniformMatrix4fv(uniformProjectionIrr, 1, GL_FALSE, glm::value_ptr(captureProjection));

	glViewport(0, 0, irradianceMap->GetWidth(), irradianceMap->GetHeight());
	irradianceMap->Write(-1);
	for (unsigned int i = 0; i < 6; ++i)
	{
		uniformViewIrr = irradianceConvolutionShader->GetViewLocation();
		glUniformMatrix4fv(uniformViewIrr, 1, GL_FALSE, glm::value_ptr(captureViews[i]));
		irradianceMap->Write(i);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		irradianceConvolutionShader->Validate();

		RenderEnvCubeMap(true);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::PrefilterPass()
{
	prefilterShader->UseShader();
	prefilterShader->SetSkybox(1);
	uniformProjectionPreF = prefilterShader->GetProjectionLocation();
	glUniformMatrix4fv(uniformProjectionPreF, 1, GL_FALSE, glm::value_ptr(captureProjection));

	prefilterMap->Write(-2, 0, 0, 0);
	unsigned int maxMipLevels = 5;

	for (unsigned int mip = 0; mip < maxMipLevels; ++mip) 
	{
		//resize framebuffer according to mip-level size
		unsigned int mipWidth = prefilterMap->GetWidth() * std::pow(0.5, mip);
		unsigned int mipHeight = prefilterMap->GetHeight() * std::pow(0.5, mip);
		prefilterMap->Write(-1, mipWidth, mipHeight, 0);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterShader->SetRoughness(roughness);
		for (unsigned int i = 0; i < 6; ++i) {
			uniformViewPreF = prefilterShader->GetViewLocation();
			glUniformMatrix4fv(uniformViewPreF, 1, GL_FALSE, glm::value_ptr(captureViews[i]));
			prefilterMap->Write(i, 0, 0, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			prefilterShader->Validate();

			RenderEnvCubeMap(true);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::BRDFPass()
{
	glViewport(0, 0, brdfMap->GetWidth(), brdfMap->GetHeight());

	brdfMap->Write();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	brdfShader->UseShader();
	quad->RenderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::DirectionalShadowMapPass(glm::mat4 viewMatrix, DirectionalLight* light) {
	
	testLitView[0] = light->CalculateCascadeLightTransform();
	mainLight->GetShadowMap()->CalcOrthProjs(camera->calculateViewMatrix(), testLitView, 60.0f);

	for (unsigned int i = 0; i < NUM_CASCADES; ++i)
	{
		vView[i] = glm::lookAt(mainLight->GetShadowMap()->GetModlCent(i), mainLight->GetShadowMap()->GetModlCent(i) + glm::normalize(light->GetLightDirection()) * 0.2f, light->GetLightUp());
	}
	for(size_t i = 0; i< NUM_CASCADES; ++i)
	{
		glViewport(0, 0, light->GetShadowMap()->GetWidth(), light->GetShadowMap()->GetHeight());
		light->GetShadowMap()->Write(i);
		glClear(GL_DEPTH_BUFFER_BIT);

		glm::mat4 projView = light->GetShadowMap()->GetProjMat(vView[i], i) * vView[i];
		
		directionalShadowShader->UseShader();
		uniformModel = directionalShadowShader->GetModelLocation();

		directionalShadowShader->SetDirectionalLightTransform(&projView);
		directionalShadowShader->Validate();

		RenderScene(directionalShadowShader);


		animDirectionalShadowShader->UseShader();

		uniformModel1 = animDirectionalShadowShader->GetModelLocation();

		animDirectionalShadowShader->SetDirectionalLightTransform(&projView);

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

void Game::OmniShadowMapPass(PointLight* light) {

	omniShadowShader->UseShader();

	glViewport(0, 0, light->GetShadowMap()->GetWidth(), light->GetShadowMap()->GetHeight());

	light->GetShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = omniShadowShader->GetModelLocation();
	uniformOmniLightPos = omniShadowShader->GetOmniLightPosLocation();
	uniformFarPlane = omniShadowShader->GetFarPlaneLocation();

	glUniform3f(uniformOmniLightPos, light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);
	glUniform1f(uniformFarPlane, light->GetFarPlane());
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

void Game::PreZPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix, GLfloat deltaTime)
{
	glViewport(0, 0,depth->GetWidth(), depth->GetHeight());
	
	depth->Write();
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
	uniformModel = static_preZPassShader->GetModelLocation();
	uniformProjection = static_preZPassShader->GetProjectionLocation();
	uniformView = static_preZPassShader->GetViewLocation();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

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

void Game::SSAOPass(glm::mat4 projectionMatrix)
{
	glViewport(0, 0, ssao->GetWidth(), ssao->GetHeight());
	ssao->Write();
	glClear(GL_COLOR_BUFFER_BIT); 

	ssaoShader->UseShader();
	
	depth->Read(GL_TEXTURE1);
	ssaoShader->SetTexture(1);

	SSAONoiseTexture->UseTexture(1);
	ssaoShader->SetNoiseTexture(2);

	quad->RenderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::SSAOBlurPass()
{
	glViewport(0, 0, ssaoBlur->GetWidth(), ssaoBlur->GetHeight());
	
	ssaoBlur->Write();
	glClear(GL_COLOR_BUFFER_BIT);

	ssaoBlurShader->UseShader();
	ssao->Read(GL_TEXTURE1);
	ssaoBlurShader->SetTexture(1);

	quad->RenderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::RenderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix, GLfloat deltaTime)
{
	glViewport(0, 0, hdr->GetWidth(), hdr->GetHeight());

	hdr->Write(); 

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mainLight->GetShadowMap()->Read(1, GL_TEXTURE2);
	irradianceMap->Read(GL_TEXTURE8);
	prefilterMap->Read(GL_TEXTURE9);
	brdfMap->Read(GL_TEXTURE10);
	ssaoBlur->Read(GL_TEXTURE14);

	skybox->DrawHDRSkybox(viewMatrix, projectionMatrix, prevProj, prevView, environmentMap.get()); //should be at the end to prevent overdraw,here becoz of blending issues

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
	terrainShader->SetPointLight(pointLights[0].get(), pointLightCount, 5, 0);
	terrainShader->SetSpotLight(spotLights[0].get(), spotLightCount, 5 + pointLightCount, pointLightCount);

	for (size_t i = 0; i < NUM_CASCADES; ++i)
	{
		glm::mat4 projView = mainLight->GetShadowMap()->GetProjMat(vView[i], i) * vView[i];
		terrainShader->SetDirectionalLightTransforms(i, &projView);
	}

	terrainShader->SetDirectionalShadowMaps(mainLight.get(), NUM_CASCADES, 2);
	terrainShader->SetAOMap(14);
	terrainShader->SetIrradianceMap(8);
	terrainShader->SetPrefilterMap(9);
	terrainShader->SetBRDFLUT(10);

	terrainShader->Validate();

	RenderTerrain(false, false);

	shaderList[0]->UseShader();

	uniformModel = shaderList[0]->GetModelLocation();
	uniformProjection = shaderList[0]->GetProjectionLocation();
	uniformView = shaderList[0]->GetViewLocation();
	uniformPrevPVM = shaderList[0]->GetPrevPVMLocation();
	uniformEyePosition = shaderList[0]->GetEyePositionLocation();
	uniformHeightScale = shaderList[0]->GetHeightScaleLocation();
	uniformAlbedoMap = shaderList[0]->GetAlbedoLocation();
	uniformMetallicMap = shaderList[0]->GetMetallicLocation();
	uniformNormalMap = shaderList[0]->GetNormalLocation();
	uniformRoughnessMap = shaderList[0]->GetRoughnessLocation();
	uniformParallaxMap = shaderList[0]->GetParallaxLocation();
	uniformGlowMap = shaderList[0]->GetGlowLocation();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(uniformEyePosition, camera->getCameraPosition().x, camera->getCameraPosition().y, camera->getCameraPosition().z);
	glUniform1f(uniformHeightScale, 0.02f);

	shaderList[0]->SetDirectionalLight(mainLight.get());
	shaderList[0]->SetPointLight(pointLights[0].get(), pointLightCount, 3, 0);
	shaderList[0]->SetSpotLight(spotLights[0].get(), spotLightCount, 3 + pointLightCount, pointLightCount);
	shaderList[0]->SetDirectionalLightTransform(mainLight->CalculateLightTransform());

	shaderList[0]->SetDirectionalShadowMap(2);
	shaderList[0]->SetIrradianceMap(8);
	shaderList[0]->SetPrefilterMap(9);
	shaderList[0]->SetBRDFLUT(10);
	shaderList[0]->SetAOMap(14);

	shaderList[0]->Validate();

	RenderScene(shaderList[0], projectionMatrix, viewMatrix);

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
	animShaderList[0]->SetPointLight(pointLights[0].get(), pointLightCount, 3, 0);
	animShaderList[0]->SetSpotLight(spotLights[0].get(), spotLightCount, 3 + pointLightCount, pointLightCount);
	animShaderList[0]->SetDirectionalLightTransform(mainLight->CalculateLightTransform());

	animShaderList[0]->SetDirectionalShadowMap(2);
	animShaderList[0]->SetAOMap(14);
	animShaderList[0]->SetIrradianceMap(8);
	animShaderList[0]->SetPrefilterMap(9);
	animShaderList[0]->SetBRDFLUT(10);

	animShaderList[0]->Validate();

	RenderAnimScene(false, false);

	billboardShader->UseShader();

	uniformProjection0 = billboardShader->GetProjectionLocation();
	uniformView0 = billboardShader->GetViewLocation();
	uniformPrevPV0 = billboardShader->GetPrevPVMLocation();
	uniformCameraUp = billboardShader->GetCameraUpLocation();
	uniformCameraRight = billboardShader->GetCameraRightLocation();
	uniformPos = billboardShader->GetPosLocation();
	uniformSize = billboardShader->GetSizeLocation();

	glUniformMatrix4fv(uniformProjection0, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView0, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(uniformCameraUp, camera->getCameraUp().x, camera->getCameraUp().y, camera->getCameraUp().z);
	glUniform3f(uniformCameraRight, camera->getCameraRight().x, camera->getCameraRight().y, camera->getCameraRight().z);

	billboardShader->SetTexture(1);

	billboardShader->Validate();

	RenderBillboardScene();

	particleShader->UseShader();

	uniformProjectionParticles = particleShader->GetProjectionLocation();
	uniformViewParticles = particleShader->GetViewLocation();
	uniformCameraUpParticles = particleShader->GetCameraUpLocation();
	uniformCameraRightParticles = particleShader->GetCameraRightLocation();

	glUniformMatrix4fv(uniformProjectionParticles, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformViewParticles, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(uniformCameraUpParticles, camera->getCameraUp().x, camera->getCameraUp().y, camera->getCameraUp().z);
	glUniform3f(uniformCameraRightParticles, camera->getCameraRight().x, camera->getCameraRight().y, camera->getCameraRight().z);

	particleShader->SetTexture(1);

	particleShader->Validate();

	RenderParticlesScene(deltaTime);

	glm::vec3 lowerLight = camera->getCameraPosition();
	lowerLight.y -= 0.1f;
	spotLights[0]->SetFlash(lowerLight, camera->getCameraDirection());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::BlurPass()
{
	bool horizontal = true;
	int amount = 10;
	blurShader->UseShader();
	for (int i = 0; i < amount; i++)
	{
		blur->Write(horizontal);
		uniformHorizontal = blurShader->GetHorizontalLocation();
		glUniform1i(uniformHorizontal, horizontal);
		blurShader->Validate();
		blurShader->SetTexture(1);
		if (i < 1)
		{
			blur->ReadFirstIteration(hdr->GetColorBuffer(1));
		}
		else
		{
			blur->Read(!horizontal);
		}
		quad->RenderQuad();
		horizontal = !horizontal;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::MotionBlurPass(float fps)
{
	glViewport(0, 0, motionBlur->GetWidth(), motionBlur->GetHeight());

	motionBlur->Write();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	motionBlurShader->UseShader();

	uniformVelocityScale = motionBlurShader->GetVelocityScaleLocation();
	glUniform1f(uniformVelocityScale, fps/30.0f);

	hdr->ReadScene(GL_TEXTURE1);
	motionBlurShader->SetTexture(1);

	hdr->ReadMotion(GL_TEXTURE2);
	motionBlurShader->SetMotionTexture(2);
	quad->RenderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::BloomPass()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	hdrShader->UseShader();
	uniformHDR = hdrShader->GetHDRLocation();
	uniformExposure = hdrShader->GetExposureLocation();
	uniformBlur = hdrShader->GetBlurLocation();

	glUniform1i(uniformHDR, 1);
	glUniform1f(uniformExposure, 1.0f);

	blur->Read(1);
	glUniform1i(uniformBlur, 1.0f);
	
	motionBlur->Read(GL_TEXTURE2);
	hdrShader->SetTexture(2);

	hdrShader->Validate();

	quad->RenderQuad();
}

Game::~Game()
{
	/*for (auto a : meshList) {
		delete a;
		a = nullptr;
	}
	for (auto a : billboardList) {
		delete a;
		a = nullptr;
	}
	for (auto a : particleList) {
	    delete a;
		a = nullptr;
	}
	if (mainLight != nullptr) {
		delete mainLight;
		mainLight = nullptr;
	}
	for(auto a: pointLights)
	{
		delete a;
		a=nullptr;
	}
	for (auto a : spotLights)
	{
		delete a;
		a = nullptr;
	}

	if (depth != nullptr) {
		delete depth;
		depth = nullptr;
	}

	if(ssao!= nullptr)
	{
		delete ssao;
		ssao = nullptr;
	}
	
	if (environmentMap != nullptr) {
		delete environmentMap;
		environmentMap = nullptr;
	}
	if (irradianceMap != nullptr) {
		delete irradianceMap;
		irradianceMap = nullptr;
	}
	if (prefilterMap != nullptr) {
		delete prefilterMap;
		prefilterMap = nullptr;
	}
	if (brdfMap != nullptr) {
		delete brdfMap;
		brdfMap = nullptr;
	}

	if (hdr != nullptr) {
		delete hdr;
		hdr = nullptr;
	}

	if (blur != nullptr) {
		delete blur;
		blur = nullptr;
	}
	if (motionBlur != nullptr)
	{
		delete motionBlur;
		motionBlur = nullptr;
	}

	if (quad != nullptr) {
		delete quad;
		quad = nullptr;
	}

	if (mesh_cube != nullptr) {
		delete mesh_cube;
		mesh_cube = nullptr;
	}

	if (skybox != nullptr) {
		delete skybox;
		skybox = nullptr;
	}*/
}