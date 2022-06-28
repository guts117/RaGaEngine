#include "pch.h"
#include "Camera.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

Camera::Camera(glm::mat4 projectionMatrix, glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
	:
	m_projectionMatrix{projectionMatrix},
	position{ startPosition },
	front{ glm::vec3(0.0f, 0.0f, -1.0f) },
	worldUp{ startUp },
	yaw{ startYaw },
	pitch{ startPitch },
	moveSpeed{ startMoveSpeed },
	turnSpeed{startTurnSpeed}

{
	update();
}

void Camera::keyControl(bool* Keys, GLfloat deltaTime) {
	if (Keys[GLFW_KEY_W]) {
		position += front * moveSpeed * deltaTime;
	}
	if (Keys[GLFW_KEY_S]) {
		position -= front * moveSpeed * deltaTime;
	}
	if (Keys[GLFW_KEY_A]) {
		position -= right * moveSpeed * deltaTime;
	}
    if (Keys[GLFW_KEY_D]) {
		position += right * moveSpeed * deltaTime;
	}
	if (Keys[GLFW_KEY_LEFT_CONTROL]) {
		position -= up * moveSpeed * deltaTime;
	}
	if (Keys[GLFW_KEY_SPACE]) {
		position += up * moveSpeed * deltaTime;
	}
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange) {
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	if (pitch < -89.0f) {
		pitch = -89.0f;
	}
	update();
}

glm::mat4 Camera::CalculateViewMatrix() {
	return glm::lookAt(position, position + glm::normalize(front), glm::normalize(up));
}

glm::mat4 Camera::CalculateShadowViewMatrix()
{
	return glm::lookAt(position, position + frontYaw, upYaw);
}

void Camera::UpdatePreviousMatrices()
{
	auto viewMatrix = CalculateViewMatrix();
	m_prevProjView = m_projectionMatrix * viewMatrix;
	m_prevProj = m_projectionMatrix;
	m_prevView = viewMatrix;
}

GLfloat Camera::GetYaw()
{
	return glm::radians(yaw);
}


glm::vec3 Camera::getCameraPosition() {
	return position;
}

glm::vec3 Camera::getCameraDirection()
{
	return glm::normalize(front);
}

glm::vec3 Camera::getCameraUp()
{
	return glm::normalize(up);
}

glm::vec3 Camera::getCameraRight()
{
	return glm::normalize(right);
}

glm::vec3 Camera::getCameraFront()
{
	return glm::normalize(front);
}

glm::mat4 Camera::GetProjectionMatrix()
{
	return m_projectionMatrix;
}

glm::mat4 Camera::GetPreviousProjectionMatrix()
{
	return m_prevProj;
}

glm::mat4 Camera::GetPreviousViewMatrix()
{
	return m_prevView;
}

glm::mat4 Camera::GetPreviousProjectionViewMatrix()
{
	return m_prevProjView;
}

void Camera::update() {
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));

	frontYaw.x = cos(glm::radians(yaw));
	frontYaw.y = 0.0f;
	frontYaw.z = sin(glm::radians(yaw));
	frontYaw = glm::normalize(frontYaw);

	upYaw = glm::normalize(glm::cross(glm::normalize(glm::cross(frontYaw, worldUp)), frontYaw));
}
