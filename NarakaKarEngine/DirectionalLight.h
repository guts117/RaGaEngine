#pragma once
#include "Light.h"

class DirectionalLight :
	public Light
{
public:
	DirectionalLight();

	DirectionalLight(GLuint shadowWidth, GLuint shadowHeight,
					GLfloat red, GLfloat green, GLfloat blue, 
					GLfloat xDir, GLfloat yDir, GLfloat zDir);

	void UseLight(GLuint ambientColorLocation, GLuint directionLocation);

	glm::mat4* CalculateLightTransform();
	glm::mat4 CalculateCascadeLightTransform();
	glm::vec3 GetLightUp();
	glm::vec3 GetLightDirection();
	~DirectionalLight();
private:
	glm::vec3 direction;
	glm::vec3 up{glm::vec3(0.0f, 1.0f, 0.0f)};
};

