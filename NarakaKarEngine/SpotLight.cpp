#include "SpotLight.h"

SpotLight::SpotLight() : PointLight() {
	direction = glm::vec3(0.0f, -1.0f, 0.0f);
	edge = 0.0f;
	procEdge = cosf(glm::radians(edge));
	isOn = true;
}

SpotLight::SpotLight(GLuint shadowWidth, GLuint shadowHeight,
					GLfloat near, GLfloat far, 
					GLfloat red, GLfloat green, GLfloat blue,
				    GLfloat xPos, GLfloat yPos, GLfloat zPos, 
					GLfloat xDir, GLfloat yDir, GLfloat zDir, 
					GLfloat edg)
				    : PointLight(shadowWidth, shadowHeight, near, far, red, green, blue, xPos, yPos, zPos){

	direction = glm::normalize(glm::vec3(xDir, yDir, zDir));
	edge = edg;
	procEdge = cosf(glm::radians(edge));
	isOn = true;
}

void SpotLight::UseLight(GLuint ambientColorLocation, 
					    GLuint positionLocation, GLuint directionLocation, 
						GLuint edgeLocation){

	if (isOn) {
		glUniform3f(ambientColorLocation, color.x, color.y, color.z);
	}
	else {
		glUniform3f(ambientColorLocation, 0.0f, 0.0f, 0.0f);
	}
	glUniform3f(positionLocation, position.x, position.y, position.z);

	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
	glUniform1f(edgeLocation, procEdge);
}

void SpotLight::SetFlash(glm::vec3 pos, glm::vec3 dir)
{
	position = pos;
	direction = dir;
}

SpotLight::~SpotLight() {
	if (shadowMap != nullptr)
	{
		delete shadowMap;
		shadowMap = nullptr;
	}
}