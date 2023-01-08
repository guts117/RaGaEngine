#include "pch.h"
#include "DirectionalLight.h"
#include "Scene_Fbo_Handler_Manager.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

extern int ScreenWidth;
extern int ScreenHeight;

DirectionalLight::DirectionalLight()
{
	direction = glm::vec3(0.0f, -1.0f, 0.0f);
	lightProj = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 20.0f);
}

DirectionalLight::DirectionalLight(GLuint shadowWidth, GLuint shadowHeight,
									GLfloat red, GLfloat green, GLfloat blue, 
									GLfloat xDir, GLfloat yDir, GLfloat zDir, 
									std::shared_ptr<Scene_Fbo_Handler_Manager> sceneFboHndlrMgr)
{
	
	direction = glm::vec3(xDir, yDir, zDir);
	lightProj = glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, 0.1f, 10000.0f);
	shadowMap = sceneFboHndlrMgr->FindFboHandler("Shadow_Map_Pass");

	color = glm::vec3(red, green, blue);
}

glm::mat4 DirectionalLight::CalculateLightTransform()
{
	glm::mat4 lTransform = lightProj * glm::lookAt(-direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	return lTransform;
}

glm::mat4 DirectionalLight::CalculateCascadeLightTransform()
{
	glm::vec3 LitDir = glm::normalize(-direction);
	glm::vec3 right = glm::normalize(glm::cross(LitDir, glm::vec3(0.0f, 1.0f, 0.0f)));
	up = glm::normalize(glm::cross(right, LitDir));
	return glm::lookAt(-direction, glm::vec3(0.0f), up);
}

float DirectionalLight::GetCascadeEnd(unsigned int i)
{
	return cascadeEnd[i];
}

void DirectionalLight::CalcOrthProjs(const glm::mat4& Cam, const glm::mat4* vView, const float& angle)
{
	float ar = float(ScreenHeight)/float(ScreenWidth);
	glm::mat4 CamInv = glm::inverse(Cam);

	float tanHalfHFOV = glm::tan(glm::radians(angle / 2.0f)) /ar;
	float tanHalfVFOV = glm::tan(glm::radians(angle * ar / 2.0f)) / ar;

	for (unsigned int i = 0; i < NUM_CASCADES; ++i)
	{
		float xn = cascadeEnd[i] * tanHalfHFOV;
		float xf = cascadeEnd[i + 1] * tanHalfHFOV;
		float yn = cascadeEnd[i] * tanHalfVFOV;
		float yf = cascadeEnd[i + 1] * tanHalfVFOV;

		glm::vec4 frustrumCorners[NUM_FRUSTUM_CORNERS] = {
			//near face
			glm::vec4(xn,yn,-cascadeEnd[i],1.0f),
			glm::vec4(-xn,yn,-cascadeEnd[i],1.0f),
			glm::vec4(xn,-yn,-cascadeEnd[i],1.0f),
			glm::vec4(-xn,-yn,-cascadeEnd[i],1.0f),
			//far Face
			glm::vec4(xf,yf,-cascadeEnd[i + 1],1.0f),
			glm::vec4(-xf,yf,-cascadeEnd[i + 1],1.0f),
			glm::vec4(xf,-yf,-cascadeEnd[i + 1],1.0f),
			glm::vec4(-xf,-yf,-cascadeEnd[i + 1],1.0f)
		};

		float minX = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::min();
		float minY = std::numeric_limits<float>::max();
		float maxY = std::numeric_limits<float>::min();
		float minZ = std::numeric_limits<float>::max();
		float maxZ = std::numeric_limits<float>::min();

		for (unsigned int j = 0; j < NUM_FRUSTUM_CORNERS; ++j)
		{
			glm::vec4 vW = CamInv * frustrumCorners[j];
			frustrumCorners[j] = vView[0] * vW;

			minX = glm::min(minX, frustrumCorners[j].x);
			maxX = glm::max(maxX, frustrumCorners[j].x);
			minY = glm::min(minY, frustrumCorners[j].y);
			maxY = glm::max(maxY, frustrumCorners[j].y);
			minZ = glm::min(minZ, frustrumCorners[j].z);
			maxZ = glm::max(maxZ, frustrumCorners[j].z);

			if (j == 7)
			{
				modeldFrusCorns[i][0] = glm::inverse(vView[0]) * glm::vec4(maxX, maxY, minZ, 1.0f);
				modeldFrusCorns[i][1] = glm::inverse(vView[0]) * glm::vec4(minX, maxY, minZ, 1.0f);
				modeldFrusCorns[i][2] = glm::inverse(vView[0]) * glm::vec4(maxX, minY, minZ, 1.0f);
				modeldFrusCorns[i][3] = glm::inverse(vView[0]) * glm::vec4(minX, minY, minZ, 1.0f);
				modeldFrusCorns[i][4] = glm::inverse(vView[0]) * glm::vec4(maxX, maxY, maxZ, 1.0f);
				modeldFrusCorns[i][5] = glm::inverse(vView[0]) * glm::vec4(minX, maxY, maxZ, 1.0f);
				modeldFrusCorns[i][6] = glm::inverse(vView[0]) * glm::vec4(maxX, minY, maxZ, 1.0f);
				modeldFrusCorns[i][7] = glm::inverse(vView[0]) * glm::vec4(minX, minY, maxZ, 1.0f);
			}
		}
	}
}

glm::mat4 DirectionalLight::GetProjMat(glm::mat4& view, unsigned int index)
{
	return glm::ortho((view * modeldFrusCorns[index][1]).x, (view * modeldFrusCorns[index][0]).x, (view * modeldFrusCorns[index][2]).y, (view * modeldFrusCorns[index][0]).y, (view * modeldFrusCorns[index][0]).z, (view * modeldFrusCorns[index][4]).z);
}

float DirectionalLight::GetRatio(glm::mat4& view, int index)
{
	if (index > -1 && index < NUM_CASCADES+1)
	{
		return((-(view * modeldFrusCorns[index][0]).z + (view * modeldFrusCorns[index][4]).z));
	}
	else
	{
		return 1.0f;
	}
}

glm::vec3 DirectionalLight::GetModlCent(unsigned int index)
{
	glm::vec4 temp = glm::vec4(0.0f);
	for (unsigned int i = 0; i < NUM_FRUSTUM_CORNERS; ++i)
	{
		temp += modeldFrusCorns[index][i];
	}
	glm::vec4 temp2 = temp / static_cast<float > (NUM_FRUSTUM_CORNERS);
	return glm::vec3(temp2);
}