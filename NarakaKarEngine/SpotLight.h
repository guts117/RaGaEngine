#pragma once
#include "PointLight.h"
class SpotLight :
	public PointLight
{
public:
	SpotLight();

	SpotLight(GLuint shadowWidth, GLuint shadowHeight,
				GLfloat near, GLfloat far, 
				GLfloat red, GLfloat green, GLfloat blue,
				GLfloat xPos, GLfloat yPos, GLfloat zPos,
				GLfloat xDir, GLfloat yDir, GLfloat zDir,
				GLfloat edg);

	void UseLight(GLuint ambientColorLocation,
		GLuint positionLocation, GLuint directionLocation,
		GLuint edgeLocation);

	void SetFlash(glm::vec3 pos, glm::vec3 dir);

	void Toggle() { isOn = !isOn; }

	~SpotLight();
private:
	glm::vec3 direction;
	
	GLfloat edge, procEdge;

	bool isOn;

};

