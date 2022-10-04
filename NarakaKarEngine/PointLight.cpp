#include "pch.h"
#include "PointLight.h"
#include "Omni_Shadow_Pass_Fbo_Handler.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

PointLight::PointLight(){

	position = glm::vec3(0.0f, 0.0f, 0.0f);
}

PointLight::PointLight(GLuint shadowWidth, GLuint shadowHeight,
						GLfloat near, GLfloat far, 
						GLfloat red, GLfloat green, GLfloat blue,
						GLfloat xPos, GLfloat yPos, GLfloat zPos) 
{
	position = glm::vec3(xPos, yPos, zPos);
	color = glm::vec3(red, green, blue);
	farPlane = far;

	float aspect = static_cast<float>(shadowWidth / shadowHeight);
	lightProj = glm::perspective(glm::radians(90.0f), aspect, near, far);

	shadowMap = new Omni_Shadow_Pass_Fbo_Handler(shadowWidth, shadowHeight);
}

void PointLight::UseLight(GLuint ambientColorLocation,
						GLuint positionLocation) {


	glUniform3f(ambientColorLocation, color.x, color.y, color.z);

	glUniform3f(positionLocation, position.x, position.y, position.z);
}

std::vector<glm::mat4> PointLight::CalculateLightTransform()
{
	std::vector<glm::mat4> lightMatrices;
	//+x, -x
	lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	//+y, -y
	lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	//+z,  -z
	lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

	return lightMatrices;
}

GLfloat PointLight::GetFarPlane()
{
	return farPlane;
}

glm::vec3 PointLight::GetPosition()
{
	return position;
}

glm::vec3 PointLight::GetColor() 
{
	return color;
}

PointLight::~PointLight() {
	if (shadowMap != nullptr)
	{
		delete shadowMap;
		shadowMap = nullptr;
	}
}