#ifndef POINTLIGHT
#define POINTLIGHT

#include "render_pch.h"

namespace NarakaRenderEngine
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

			glm::vec3 position;
			GLfloat farPlane{ 0.0f };
			glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);;
			glm::mat4 lightProj = glm::mat4();
	
			Fbo_Handler* GetShadowMap() { return shadowMap; }
			
			~PointLight() = default;
		protected:
			Fbo_Handler* shadowMap;
		};
	}
}
#endif