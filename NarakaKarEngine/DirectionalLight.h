#ifndef DIRECTIONALLIGHT
#define DIRECTIONALLIGHT

#include "pch.h"
#include "Shadow_Map_Pass_Fbo_Handler.h"
#include "RenderingCommonValues.h"

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
			Shadow_Map_Pass_Fbo_Handler* GetShadowMap() { return shadowMap; }
			float GetCascadeEnd(unsigned int i);
			void CalcOrthProjs(const glm::mat4& Cam, const glm::mat4* vView, const float& angle);
			glm::mat4 GetProjMat(glm::mat4& view, unsigned int index);
			float GetRatio(glm::mat4& view, int index);
			glm::vec3 GetModlCent(unsigned int index);
			~DirectionalLight();
		private:
			glm::vec3 direction;
			glm::vec3 up{ glm::vec3(0.0f, 1.0f, 0.0f) };
			glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);;
			glm::mat4 lightProj = glm::mat4();

			GLfloat cascadeEnd[NUM_CASCADES + 1] = { 0.1f,  50.0f,  200.0f, 1000.0f/*, 1000.0f*/ };
			glm::mat4 shadowOrthoProjInfo[NUM_CASCADES];
			glm::vec4 modeldFrusCorns[NUM_CASCADES][NUM_FRUSTUM_CORNERS];

			Shadow_Map_Pass_Fbo_Handler* shadowMap = nullptr;
		};
	}
}
#endif