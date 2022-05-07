#include "Skybox.h"

Skybox::Skybox()
{
	skyShader = new Model_Shader();
	skyShader->CreateFromFiles("Shaders/skybox.vert", "Shaders/skybox.frag");

	uniformProjection = skyShader->GetProjectionLocation();
	uniformView = skyShader->GetViewLocation();
	uniformPrevPV = skyShader->GetPrevPVMLocation();

	skyMesh = new Static_Mesh();

	//// Mesh setup
	//unsigned int skyboxIndices[] = {
	//	//front
	//	0,1,2,
	//	2,1,3,
	//	//right
	//	2,3,5,
	//	5,3,7,
	//	//back
	//	5,7,4,
	//	4,7,6,
	//	//left
	//	4,6,0,
	//	0,6,1,
	//	//top
	//	4,0,5,
	//	5,0,2,
	//	//bottom
	//	1,6,3,
	//	3,6,7
	//};

	//float skyboxVertices[] = {
	//	-1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	//	-1.0f, -1.0f, -1.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	//	1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	//	1.0f, -1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,

	//	-1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	//	1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	//	-1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	//	1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f

	//};

	//skyMesh = new Static_Mesh();
	//skyMesh->CreateMesh(skyboxVertices, skyboxIndices, 64, 36);
}

Skybox::Skybox(std::vector<std::string> faceLocation)
{
	//shader setup
	skyShader = new Model_Shader();
	skyShader->CreateFromFiles("Shaders/skybox.vert", "Shaders/skybox.frag");

	uniformProjection = skyShader->GetProjectionLocation();
	uniformView = skyShader->GetViewLocation();
	uniformPrevPV = skyShader->GetPrevPVMLocation();

	skyMesh = new Static_Mesh();
	//texture setup
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	int width, height, bitDepth;
	for (size_t i = 0; i < 6; i++)
	{
		unsigned char* texData = stbi_load(faceLocation[i].c_str(), &width, &height, &bitDepth, 0);

		if (!texData) {
			printf("Failed ot find: %s\n", faceLocation[i].c_str());
			return;
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
		stbi_image_free(texData);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Skybox::DrawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::mat4 prevP, glm::mat4 prevV)
{
	viewMatrix = glm::mat4(glm::mat3(viewMatrix));
	prevV = glm::mat4(glm::mat3(prevV));

	glm::mat4 prevPV = glm::mat4();

	//glDepthMask(GL_FALSE);

	glDepthFunc(GL_LEQUAL); //so that skybox doesn't render on top

	skyShader->UseShader();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	prevPV = prevP*prevV;
	glUniformMatrix4fv(uniformPrevPV, 1, GL_FALSE, glm::value_ptr(prevPV));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	skyShader->Validate();

	skyMesh->RenderCube();

	//glDepthMask(GL_TRUE);
}

void Skybox::DrawHDRSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::mat4 prevP, glm::mat4 prevV, Equirectangular_to_CubeMap_Framebuffer* envMap)
{
	viewMatrix = glm::mat4(glm::mat3(viewMatrix));
	prevV = glm::mat4(glm::mat3(prevV));

	glm::mat4 prevPV = glm::mat4();

	glDepthFunc(GL_LEQUAL); //so that skybox doesn't render on top

	skyShader->UseShader();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	prevPV = prevP * prevV;
	glUniformMatrix4fv(uniformPrevPV, 1, GL_FALSE, glm::value_ptr(prevPV));

	envMap->Read(GL_TEXTURE1);
	skyShader->SetSkybox(1);

	skyShader->Validate();

	skyMesh->RenderCube();
}

Skybox::~Skybox()
{
	if (skyShader != nullptr) {
		delete skyShader;
		skyShader = nullptr;
	}
	if (skyMesh != nullptr) {
		delete skyMesh;
		skyMesh = nullptr;
	}
	if(textureId)
	{
		glDeleteTextures(1, &textureId);
	}
}
