#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	Camera() = default;
	Camera(glm::mat4 projectionMatrix, glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);

	void keyControl(bool* Keys, GLfloat deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange);

	glm::vec3 getCameraPosition();
	glm::vec3 getCameraDirection();
	glm::vec3 getCameraUp();
	glm::vec3 getCameraRight();
	glm::vec3 getCameraFront();

	glm::mat4 GetProjectionMatrix();
	glm::mat4 GetPreviousProjectionMatrix();
	glm::mat4 GetPreviousViewMatrix();
	glm::mat4 GetPreviousProjectionViewMatrix();

	glm::mat4 CalculateViewMatrix();
	glm::mat4 CalculateShadowViewMatrix();
	void UpdatePreviousMatrices();

	GLfloat GetYaw();

	~Camera() = default;
private:
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 front = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 right = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 left = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 worldUp = glm::vec3(0.0f, 0.0f, 0.0f);

	GLfloat yaw = 0.0f;
	GLfloat pitch = 0.0f;

	GLfloat moveSpeed = 0.0f;
	GLfloat turnSpeed = 0.0f;

	glm::vec3 frontYaw = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 upYaw = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 m_projectionMatrix = glm::mat4(1.0f);

	glm::mat4 m_prevProjView = glm::mat4(1.0f);

	glm::mat4 m_prevProj = glm::mat4(1.0f);
	glm::mat4 m_prevView = glm::mat4(1.0f);

	void update();
};

