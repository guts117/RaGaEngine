#include "pch.h"
#include "Light.h"
#include "ShadowMap_Framebuffer.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

Light::Light() {
	//empty
}

Light::Light(GLuint shadowWidth, GLuint shadowHeight,
			GLfloat red, GLfloat green, GLfloat blue) {
	shadowMap = new ShadowMap_Framebuffer();
	shadowMap->Init(shadowWidth, shadowHeight);

	color = glm::vec3(red, green, blue);
}

ShadowMap_Framebuffer* Light::GetShadowMap()
{
	return shadowMap;
}

Light::~Light() 
{
}