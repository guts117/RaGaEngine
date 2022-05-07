#pragma once

#include "Framebuffer.h"

class ShadowMap_Framebuffer:public Framebuffer
{
public:
	ShadowMap_Framebuffer() = default;

	bool Init(GLuint width, GLuint height);
	void Write();  //for omni-Shadow map
	void Write(int CascadeIndex);
	virtual void Read(int i, GLenum textureUnit);

	float GetCascadeEnd(unsigned int i);
	void CalcOrthProjs(const glm::mat4& Cam, const glm::mat4* vView, const float& angle);
	glm::mat4 GetProjMat(glm::mat4& view, unsigned int index);
	float GetRatio(glm::mat4& view, int index);
	glm::vec3 GetModlCent(unsigned int index);
	
	~ShadowMap_Framebuffer();
private:
	GLuint buffers[NUM_CASCADES] = { 0 };
	GLuint fbo[NUM_CASCADES] = { 0 };

	GLfloat cascadeEnd[NUM_CASCADES + 1] = { 0.1f,  50.0f,  200.0f, 1000.0f/*, 1000.0f*/};
	glm::mat4 shadowOrthoProjInfo[NUM_CASCADES];
	glm::vec4 modeldFrusCorns[NUM_CASCADES][NUM_FRUSTUM_CORNERS];
};