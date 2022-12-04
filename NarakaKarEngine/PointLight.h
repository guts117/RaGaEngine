#ifndef POINTLIGHT
#define POINTLIGHT

#include "pch.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Fbo_Handler;
		class Scene_Fbo_Handler_Manager;

		class PointLight
		{
		public:
			PointLight();
			PointLight(GLuint shadowWidth, GLuint shadowHeight,
				GLfloat near, GLfloat far,
				GLfloat red, GLfloat green, GLfloat blue,
				GLfloat xPos, GLfloat yPos, GLfloat zPos,
				std::shared_ptr<Scene_Fbo_Handler_Manager> sceneFboHndlrMgr);

			void UseLight(GLuint ambientColorLocation,
				GLuint positionLocation);

			std::vector<glm::mat4> CalculateLightTransform();
			GLfloat GetFarPlane();
			glm::vec3 GetPosition();
			glm::vec3 GetColor();
			std::shared_ptr<Fbo_Handler> GetShadowMap() { return shadowMap; }
			~PointLight() = default;

		protected:
			glm::vec3 position;
			GLfloat farPlane{ 0.0f };
			glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);;
			glm::mat4 lightProj = glm::mat4();
			std::shared_ptr<Fbo_Handler> shadowMap;

		};
	}
}
#endif