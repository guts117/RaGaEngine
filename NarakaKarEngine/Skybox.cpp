#include "pch.h"
#include "Skybox.h"
#include "Fbo_Handler.h"
#include "Model_Shader.h"
#include "Static_Mesh.h"
#include "Texture.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

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

	auto loc = faceLocation[0];
	for (int i = 1; i < 6; ++i) { loc += "\n" + faceLocation[i]; }

	cubeMap = new Texture(loc, true);
	cubeMap->LoadCubeMap();
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

	cubeMap->UseCubeMap(0);

	skyShader->Validate();

	skyMesh->RenderCube();

	//glDepthMask(GL_TRUE);
}

void Skybox::DrawHDRSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::mat4 prevP, glm::mat4 prevV, std::shared_ptr<Fbo_Handler> envMap)
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

	envMap->AttachFBOToTextureUnit(0, GL_TEXTURE1, 0, 0);
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
	if(cubeMap != nullptr)
	{
		delete cubeMap;
		cubeMap = nullptr;
	}
}
