#ifndef LIGHT
#define LIGHT

#include "pch.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class ShadowMap_Framebuffer;

		class Light
		{
		public:
			Light();
			Light(GLuint shadowWidth, GLuint shadowHeight,
				GLfloat red, GLfloat green, GLfloat blue);

			ShadowMap_Framebuffer* GetShadowMap();

			virtual ~Light() = 0;

		protected:
			glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);;

			glm::mat4 lightProj = glm::mat4();

			ShadowMap_Framebuffer* shadowMap = nullptr;
		};
	}
}
#endif