#ifndef DIRECTIONALLIGHT
#define DIRECTIONALLIGHT

#include "pch.h"
#include "ShadowMap_Framebuffer.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class DirectionalLight
		{
		public:
			DirectionalLight();

			DirectionalLight(GLuint shadowWidth, GLuint shadowHeight,
				GLfloat red, GLfloat green, GLfloat blue,
				GLfloat xDir, GLfloat yDir, GLfloat zDir);

			void UseLight(GLuint ambientColorLocation, GLuint directionLocation);

			glm::mat4 CalculateLightTransform();
			glm::mat4 CalculateCascadeLightTransform();
			glm::vec3 GetLightUp();
			glm::vec3 GetLightDirection();
			ShadowMap_Framebuffer* GetShadowMap() { return shadowMap; }
			~DirectionalLight();
		private:
			glm::vec3 direction;
			glm::vec3 up{ glm::vec3(0.0f, 1.0f, 0.0f) };
			glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);;
			glm::mat4 lightProj = glm::mat4();
			ShadowMap_Framebuffer* shadowMap = nullptr;
		};
	}
}
#endif