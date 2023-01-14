#include "pch.h"
#include "PointLight.h"
#include "Scene_Fbo_Handler_Manager.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

PointLight::PointLight(){

	position = glm::vec3(0.0f, 0.0f, 0.0f);
}

PointLight::PointLight(GLuint shadowWidth, GLuint shadowHeight,
						GLfloat near, GLfloat far, 
						GLfloat red, GLfloat green, GLfloat blue,
						GLfloat xPos, GLfloat yPos, GLfloat zPos,
						std::shared_ptr<Scene_Fbo_Handler_Manager> sceneFboHndlrMgr)
{
	position = glm::vec3(xPos, yPos, zPos);
	color = glm::vec3(red, green, blue);
	farPlane = far;

	float aspect = static_cast<float>(shadowWidth / shadowHeight);
	lightProj = glm::perspective(glm::radians(90.0f), aspect, near, far);

	shadowMap = sceneFboHndlrMgr->FindFboHandler("Omni_Shadow_Map_Pass");
}