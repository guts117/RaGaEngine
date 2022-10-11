#ifndef SKYBOX
#define SKYBOX

#include "pch.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Model_Shader;
		class Fbo_Handler;
		class Static_Mesh;
		class Texture;

		class Skybox
		{
		public:
			Skybox();
			Skybox(std::vector<std::string> faceLocation);

			void DrawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::mat4 prevP, glm::mat4 prevV);

			void DrawHDRSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::mat4 prevP, glm::mat4 prevV, std::shared_ptr<Fbo_Handler> envMap);
			~Skybox();

		private:
			Static_Mesh* skyMesh = nullptr;
			Model_Shader* skyShader = nullptr;

			Texture* cubeMap;
			GLuint uniformProjection{ 0 }, uniformView{ 0 }, uniformPrevPV{ 0 };
		};
	}
}
#endif