#ifndef DIRECTIONAL_LIGHT
#define DIRECTIONAL_LIGHT

#include "render_pch.h"
#include "RenderingCommonValues.h"

namespace NarakaRenderEngine
{
	namespace RenderEngine
	{
		class Fbo_Handler;
		class Scene_Fbo_Handler_Manager;

		class DirectionalLight
		{
		public:
			DirectionalLight();

			DirectionalLight(GLuint shadowWidth, GLuint shadowHeight,
				GLfloat red, GLfloat green, GLfloat blue,
				GLfloat xDir, GLfloat yDir, GLfloat zDir, 
				std::shared_ptr<Scene_Fbo_Handler_Manager> sceneFboHndlrMgr);

			glm::mat4 CalculateLightTransform();
			glm::mat4 CalculateCascadeLightTransform();
			std::shared_ptr<Fbo_Handler> GetShadowMap() { return shadowMap; }
			float GetCascadeEnd(unsigned int i);
			void CalcOrthProjs(const glm::mat4& Cam, const glm::mat4* vView, const float& angle, const glm::ivec2& screenDims);
			glm::mat4 GetProjMat(glm::mat4& view, unsigned int index);
			float GetRatio(glm::mat4& view, int index);
			glm::vec3 GetModlCent(unsigned int index);
			~DirectionalLight() = default;

			glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);;
			glm::vec3 direction;
			glm::vec3 up{ glm::vec3(0.0f, 1.0f, 0.0f) };
		private:
			glm::mat4 lightProj = glm::mat4();

			GLfloat cascadeEnd[NUM_CASCADES + 1] = { 0.1f,  50.0f,  200.0f, 1000.0f/*, 1000.0f*/ };
			glm::mat4 shadowOrthoProjInfo[NUM_CASCADES];
			glm::vec4 modeldFrusCorns[NUM_CASCADES][NUM_FRUSTUM_CORNERS];

			std::shared_ptr<Fbo_Handler> shadowMap;
		};
	}
}
#endif