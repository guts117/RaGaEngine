#ifndef POINTLIGHT
#define POINTLIGHT

#include "pch.h";
#include "Omni_Shadow_Pass_Fbo_Handler.h";

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class PointLight
		{
		public:
			PointLight();
			PointLight(GLuint shadowWidth, GLuint shadowHeight,
				GLfloat near, GLfloat far,
				GLfloat red, GLfloat green, GLfloat blue,
				GLfloat xPos, GLfloat yPos, GLfloat zPos);

			void UseLight(GLuint ambientColorLocation,
				GLuint positionLocation);

			std::vector<glm::mat4> CalculateLightTransform();
			GLfloat GetFarPlane();
			glm::vec3 GetPosition();
			glm::vec3 GetColor();
			Omni_Shadow_Pass_Fbo_Handler* GetShadowMap() { return shadowMap; }
			~PointLight();

		protected:
			glm::vec3 position;
			GLfloat farPlane{ 0.0f };
			glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);;
			glm::mat4 lightProj = glm::mat4();
			Omni_Shadow_Pass_Fbo_Handler* shadowMap = nullptr;

		};
	}
}
#endif