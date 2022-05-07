#include "Light.h"

Light::Light() {
	//empty
}

Light::Light(GLuint shadowWidth, GLuint shadowHeight,
			GLfloat red, GLfloat green, GLfloat blue) {
	shadowMap = new ShadowMap_Framebuffer();
	shadowMap->Init(shadowWidth, shadowHeight);

	color = glm::vec3(red, green, blue);
}

Light::~Light() {
}