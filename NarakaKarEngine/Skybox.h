#ifndef SKYBOX
#define SKYBOX

#include "pch.h"

class Model_Shader;
class Equirectangular_to_CubeMap_Framebuffer;
class Static_Mesh;
class Texture;

class Skybox
{
public:
	Skybox();
	Skybox(std::vector<std::string> faceLocation);

	void DrawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::mat4 prevP, glm::mat4 prevV);

	void DrawHDRSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::mat4 prevP, glm::mat4 prevV, Equirectangular_to_CubeMap_Framebuffer* envMap);
	~Skybox();

private:
	Static_Mesh* skyMesh = nullptr;
	Model_Shader* skyShader = nullptr;
	
	Texture* cubeMap;
	GLuint uniformProjection{ 0 }, uniformView{ 0 }, uniformPrevPV{0};
};

#endif