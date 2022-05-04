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
	mainWindow->Initialise();
	CreateBillboard();
	CreateParticles();
	CreateTerrain();
	CreateObject();
	CreateShaders();

	auto projectionMatrix = glm::perspective(glm::radians(60.0f), (GLfloat)mainWindow->getBufferWidth() / (GLfloat)mainWindow->getBufferHeight(), camNearZ, camFarZ);
	camera = std::make_shared<Camera>(projectionMatrix, glm::vec3(-terrainScaleFactor, 30.0f, -terrainScaleFactor), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 50.0f, 0.2f);

	environmentTexture = std::make_unique<Texture>("Textures/HDR/GCanyon_C_YumaPoint_3k.hdr");
	environmentTexture->LoadTextureHDR();

	plainTexture = std::make_unique <Texture>("Textures/plain.png");
	plainTexture->LoadTextureSRGB();
	
	grassTexture = std::make_unique <Texture>("Textures/grass.png");
	grassTexture->LoadTextureSRGBA();


	pyramid1 = std::make_unique<Static_Object>();
	pyramid1->SetUpNativeModelData(meshList[0],"Textures/rustediron2.png","Textures/Metallic/rustediron2.png", 
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

	bulbWhite = std::make_unique<Static_Object>();
	bulbWhite->SetUpImportedModelData("Models/Free_Antique_Bulb.obj");
	bulbRed = std::make_unique<Static_Object>();
	bulbRed->SetUpImportedModelData("Models/Free_Antique_Bulb.obj");
	sphere = std::make_unique<Static_Object>();
	sphere->SetUpImportedModelData("Models/sphere.obj");
	cube = std::make_unique<Static_Object>();
	cube->SetUpImportedModelData("Models/cube.obj");
	sniper = std::make_unique<Static_Object>();
	sniper->SetUpImportedModelData("Models/Sniper_rifle_KSR-29.fbx");
	gun = std::make_unique<Static_Object>();
	gun->SetUpImportedModelData("Models/Cerberus_LP.fbx");
	//anymodel = std::make_unique<Static_Object>();
	////anymodel->SetUpImportedModelData("Models/Intergalactic_Spaceship-(Wavefront).obj");
	//anymodel->SetUpImportedModelData("Models/Sponza.gltf");

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
		0.1f, 0.1f, 0.1f,
		5500.0f, -5500.0f, -10000.0f);

	pointLights[0] = std::make_shared < PointLight>(1024, 1024,
		0.1f, 100.0f,
		0.0f, 0.0f, 3.0f,
		12.0f - terrainScaleFactor, 40.0f, 10.0f - terrainScaleFactor);

	pointLightCount++;

	pointLights[1] = std::make_shared < PointLight>(1024, 1024,
		0.1f, 100.0f,
		3.0f, 0.0f, 0.0f,
		-12.0f - terrainScaleFactor, 40.0f, 10.0f - terrainScaleFactor);

	pointLightCount++;

	//pointLights[2] = std::make_shared < PointLight>(1024, 1024,
	//	0.1f, 100.0f,
	//	0.0f, 10.0f, 0.0f,
	//	-6.0f - terrainScaleFactor, 40.0f, 10.0f - terrainScaleFactor);

	//pointLightCount++;

	spotLights[0] = std::make_shared < SpotLight>(1024, 1024,
		0.1f, 100.0f,
		10.0f, 10.0f, 10.0f,
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

	//SSAO initialization
	ssaoShader->UseShader();

	uniformSampleRadius = ssaoShader->GetSampleRadiusLocation();
	uniformProjectionAO = ssaoShader->GetProjectionLocation();

	glUniformMatrix4fv(uniformProjectionAO, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
	glUniform1f(uniformSampleRadius, 0.1f);

	ssaoShader->GenKernel();
	ssaoShader->GenNoise(ssaoNoiseData);

	SSAONoiseTexture->GenerateNoiseTexture(ssaoNoiseData);

	terrainShader->UseShader();
	for (size_t i = 0; i < NUM_CASCADES; ++i)
	{
		glm::vec4 vView(0.0f, 0.0f, mainLight->GetShadowMap()->GetCascadeEnd(i+1), 1.0f);
		glm::vec4 vClip = camera->GetProjectionMatrix() * vView;
		printf("%F \n", vClip.z);
		terrainShader->SetCascadeEndClipSpace(i, -vClip.z);
	}

	InitSSBOs();
	CreateClusters();
	EnvironmentMapPass();
	IrradianceConvolutionPass();
	PrefilterPass();
	BRDFPass();
}

void Game::InitSSBOs() {
	//Setting up tile size on both X and Y 
	sizeX = (unsigned int)std::ceilf(ScreenWidth / (float)gridSizeX);

	float zFar = camFarZ;
	float zNear = camNearZ;

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
		screen2View.tileSizes[3] = sizeX;
		screen2View.screenWidth = ScreenWidth;
		screen2View.screenHeight = ScreenHeight;
		//Basically reduced a log function into a simple multiplication an addition by pre-calculating these
		float factor = gridSizeZ / glm::log(zFar / zNear);
		screen2View.sliceScalingFactor = factor;
		screen2View.sliceBiasFactor = -(glm::log(zNear) * factor);
		screen2View.zNear = camNearZ;
		screen2View.zFar = camFarZ;
		//Generating and copying data to memory in GPU
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(struct ScreenToView), &screen2View, GL_STATIC_COPY);
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
			lightList.get()->at(i).range = camFarZ;
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

	//Setting up simplest ssbo in the world
	{
		glGenBuffers(1, &lightIndexGlobalCountSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightIndexGlobalCountSSBO);

		//Every tile takes two unsigned ints one to represent the number of lights in that grid
		//Another to represent the offset 
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int), NULL, GL_STATIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, lightIndexGlobalCountSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	{
		glGenBuffers(1, &visibleClusterSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, visibleClusterSSBO);

		glBufferData(GL_SHADER_STORAGE_BUFFER, numClusters * sizeof(bool), NULL, GL_STATIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, visibleClusterSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	{
		glGenBuffers(1, &uniqueClusterSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, uniqueClusterSSBO);

		glBufferData(GL_SHADER_STORAGE_BUFFER, numClusters * sizeof(unsigned int), NULL, GL_STATIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, uniqueClusterSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	{
		glGenBuffers(1, &activeClusterCountSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, activeClusterCountSSBO);

		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int), NULL, GL_STATIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, activeClusterCountSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}

void Game::CreateClusters() {
	//Building the grid of AABB enclosing the view frustum clusters
	buildAABBGridCompShader->UseShader();
	buildAABBGridCompShader->Dispatch(gridSizeX, gridSizeY, gridSizeZ);
}

void Game::update(float fps) {
	GLfloat now = static_cast<GLfloat>(glfwGetTime()); //SDL_GetPerformanceCounter();
	deltaTime = now - lastTime;	//(now-lastTime)*1000/SDL_GetPerfomnaceFrequency(); //for seconds
	lastTime = now;

	//get + handle user input events
	glfwPollEvents();
	camera->keyControl(mainWindow->getKeys(), deltaTime);
	camera->mouseControl(mainWindow->getXChange(), mainWindow->getYChange());

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
		OmniShadowMapPass(pointLights[i].get());
	}
	for (size_t i = 0; i < spotLightCount; i++) {
		OmniShadowMapPass(spotLights[i].get());
	}

	PreZPass(deltaTime);
	CullLight();
	SSAOPass();
	SSAOBlurPass();
	RenderPass(deltaTime);
	BlurPass();
	MotionBlurPass(fps);
	BloomPass();


	//prevProjView = projection *camera->CalculateViewMatrix();
	//prevProj = projection;
	//prevView = camera->CalculateViewMatrix();
	camera->UpdatePreviousMatrices();
	glUseProgram(0);

	mainWindow->swapBuffers();
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
		
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;

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
		vertices[in0] += Tangent.x; vertices[in0 + 1] += Tangent.y; vertices[in0 + 2] += Tangent.z;
		vertices[in1] += Tangent.x; vertices[in1 + 1] += Tangent.y; vertices[in1 + 2] += Tangent.z;
		vertices[in2] += Tangent.x; vertices[in2 + 1] += Tangent.y; vertices[in2 + 2] += Tangent.z;
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

	//Debug::DebugPrintReferenceTBN("ReferenceTBN", terrainVertices, 11, glm::vec3(0.0f, 1.0f, 0.0f));
	//Debug::DebugPrintTBN("TerrainTBN", terrainVertices, 5, 8);

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

	buildAABBGridCompShader->CreateFromFiles("Shaders/clusterShader.comp");
	visibleClusterCompShader->CreateFromFiles("Shaders/clusterVisibleShader.comp");
	uniqueClusterCompShader->CreateFromFiles("Shaders/clusterUniqueShader.comp");
	cullLightsCompShader->CreateFromFiles("Shaders/clusterCullLightShader.comp");

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

	prevPV = camera->GetPreviousProjectionViewMatrix();
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
	prevPVM = camera->GetPreviousProjectionViewMatrix() * terrainList[0]->prevMesh;
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

void Game::RenderScene(std::shared_ptr<Shader> shader) {
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

	sphere->Translate(-terrainScaleFactor, 35.0f, 5.5f - terrainScaleFactor);
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

	//anymodel->Translate(-terrainScaleFactor, 37.0f, 1.0f - terrainScaleFactor);
	//anymodel->Scale(1.0f, 1.0f, 1.0f);
	//anymodel->DrawImportedObject(shader, camera);
}

void Game::RenderAnimScene(bool shadow, bool depth) {
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
	prevPVM = camera->GetPreviousProjectionViewMatrix() * anim2->prevModel;
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

void Game::DirectionalShadowMapPass(DirectionalLight* light) {
	
	testLitView[0] = light->CalculateCascadeLightTransform();
	mainLight->GetShadowMap()->CalcOrthProjs(camera->CalculateViewMatrix(), testLitView, 60.0f);

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

void Game::CullLight()
{
	visibleClusterCompShader->UseShader();
	depth->Read(GL_TEXTURE0);
	visibleClusterCompShader->Dispatch(ScreenWidth / 32, ScreenHeight / 30, 1);

	uniqueClusterCompShader->UseShader();
	uniqueClusterCompShader->Dispatch(gridSizeX, gridSizeY, gridSizeZ);

	unsigned int count;
	glGetNamedBufferSubData(activeClusterCountSSBO, 0, sizeof(unsigned int), &count);

	//std::cout << count << " Clusters are Active" << std::endl;

	//4-Light assignment
	cullLightsCompShader->UseShader();
	glUniformMatrix4fv(cullLightsCompShader->GetViewLocation(), 1, GL_FALSE, glm::value_ptr(camera->CalculateViewMatrix()));
	cullLightsCompShader->Dispatch(count, 1, 1);
}

void Game::PreZPass(GLfloat deltaTime)
{
	auto projectionMatrix = camera->GetProjectionMatrix();
	auto viewMatrix = camera->CalculateViewMatrix();
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

void Game::SSAOPass()
{
	auto projectionMatrix = camera->GetProjectionMatrix();

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

void Game::RenderPass(GLfloat deltaTime)
{
	auto projectionMatrix = camera->GetProjectionMatrix();
	auto viewMatrix = camera->CalculateViewMatrix();
	auto prevProj = camera->GetPreviousProjectionMatrix();
	auto prevView = camera->GetPreviousViewMatrix();

	glViewport(0, 0, hdr->GetWidth(), hdr->GetHeight());

	hdr->Write(); 

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mainLight->GetShadowMap()->Read(1, GL_TEXTURE2);
	irradianceMap->Read(GL_TEXTURE8);
	prefilterMap->Read(GL_TEXTURE9);
	brdfMap->Read(GL_TEXTURE10);
	ssaoBlur->Read(GL_TEXTURE14);
	depth->Read(GL_TEXTURE18);

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
	terrainShader->SetPointLight(pointLights, pointLightCount, 5, 0);
	terrainShader->SetSpotLight(spotLights, spotLightCount, 5 + pointLightCount, pointLightCount);

	for (size_t i = 0; i < NUM_CASCADES; ++i)
	{
		glm::mat4 projView = mainLight->GetShadowMap()->GetProjMat(vView[i], i) * vView[i];
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
	shaderList[0]->SetPointLight(pointLights, pointLightCount, 3, 0);
	shaderList[0]->SetSpotLight(spotLights, spotLightCount, 3 + pointLightCount, pointLightCount);
	shaderList[0]->SetDirectionalLightTransform(mainLight->CalculateLightTransform());

	shaderList[0]->SetDirectionalShadowMap(2);
	shaderList[0]->SetIrradianceMap(8);
	shaderList[0]->SetPrefilterMap(9);
	shaderList[0]->SetBRDFLUT(10);
	shaderList[0]->SetAOMap(14);
	shaderList[0]->SetDepthMap(18);

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
	animShaderList[0]->SetDirectionalLightTransform(mainLight->CalculateLightTransform());

	animShaderList[0]->SetDirectionalShadowMap(2);
	animShaderList[0]->SetAOMap(14);
	animShaderList[0]->SetDepthMap(18);
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
	if (lightIndexGlobalCountSSBO != 0)
	{
		glDeleteBuffers(1, &lightIndexGlobalCountSSBO);
		lightIndexGlobalCountSSBO = 0;
	}
	if (visibleClusterSSBO != 0)
	{
		glDeleteBuffers(1, &visibleClusterSSBO);
		visibleClusterSSBO = 0;
	}
	if (uniqueClusterSSBO != 0)
	{
		glDeleteBuffers(1, &uniqueClusterSSBO);
		uniqueClusterSSBO = 0;
	}
	if (activeClusterCountSSBO != 0)
	{
		glDeleteBuffers(1, &activeClusterCountSSBO);
		activeClusterCountSSBO = 0;
	}
}