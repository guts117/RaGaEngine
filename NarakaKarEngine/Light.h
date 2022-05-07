#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 

#include "ShadowMap_Framebuffer.h"

class Light
{
public:
	Light();
	Light(GLuint shadowWidth, GLuint shadowHeight,
			GLfloat red, GLfloat green, GLfloat blue);

	ShadowMap_Framebuffer* GetShadowMap() { return shadowMap; }

	virtual ~Light() = 0;

protected:
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);;

	glm::mat4 lightProj = glm::mat4();

	ShadowMap_Framebuffer* shadowMap = nullptr;
};

