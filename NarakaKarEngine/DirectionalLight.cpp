#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Light() {
	
	direction = glm::vec3(0.0f, -1.0f, 0.0f);
	lightProj = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 20.0f);
}

DirectionalLight::DirectionalLight(GLuint shadowWidth, GLuint shadowHeight,
									GLfloat red, GLfloat green, GLfloat blue, 
									GLfloat xDir, GLfloat yDir, GLfloat zDir)
									: Light(shadowWidth, shadowHeight, red, green, blue) {
	
	direction = glm::vec3(xDir, yDir, zDir);
	lightProj = glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, 0.1f, 10000.0f);
}

void DirectionalLight::UseLight(GLuint ambientColorLocation,GLuint directionLocation) {

	glUniform3f(ambientColorLocation, color.x, color.y, color.z);
	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
}

glm::mat4* DirectionalLight::CalculateLightTransform()
{
	glm::mat4 lTransform = lightProj * glm::lookAt(-direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	return &lTransform;
}

glm::mat4 DirectionalLight::CalculateCascadeLightTransform()
{
	glm::vec3 LitDir = glm::normalize(-direction);
	glm::vec3 right = glm::normalize(glm::cross(LitDir, glm::vec3(0.0f, 1.0f, 0.0f)));
	up = glm::normalize(glm::cross(right, LitDir));
	return glm::lookAt(-direction, glm::vec3(0.0f), up);
}

glm::vec3 DirectionalLight::GetLightUp()
{
	return up;
}

glm::vec3 DirectionalLight::GetLightDirection()
{
	return -direction;
}

DirectionalLight::~DirectionalLight() {
	if (shadowMap != nullptr)
	{
		delete shadowMap;
		shadowMap = nullptr;
	}
}