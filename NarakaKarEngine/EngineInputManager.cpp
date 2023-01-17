#include "pch.h"
#include "EngineInputManager.h"
#include "EngineUIMain.h"

using namespace NarakaKarEngine;
using namespace InputManager;
using namespace EngineUI;


double EngineInputManager::scrollVal;
bool EngineInputManager::keys[1024];

bool EngineInputManager::isLeftMousePress;
bool EngineInputManager::isLeftMouseRelease;
bool EngineInputManager::isMiddleMousePress;
bool EngineInputManager::isMiddleMouseRelease;

GLfloat EngineInputManager::lastX;
GLfloat EngineInputManager::lastY;
GLfloat EngineInputManager::xChange;
GLfloat EngineInputManager::yChange;
bool EngineInputManager::mouseFirstMoved;

EngineInputManager::EngineInputManager()
{
	for (size_t i = 0; i < 1024; i++) {
		keys[i] = 0;
	}
}

GLfloat EngineInputManager::GetXChange() {
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat  EngineInputManager::GetYChange() {
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

glm::vec2 EngineInputManager::GetCursorPos()
{
	return glm::vec2(lastX, lastY);
}

void EngineInputManager::HandleKeysPresses(GLFWwindow* window, int key, int code, int action, int mode) 
{
	//ToDo: Key inputs on Imgui Windows.
	if (EngineUIMain::AddKeyBoardButtonEvent(ImGuiKey::ImGuiKey_None, false))
	{
		if (key >= 0 && key < 1024) {
			if (action == GLFW_PRESS) {
				keys[key] = true;
			}
			else if (action == GLFW_RELEASE) {
				keys[key] = false;
			}
		}
	}
}
void EngineInputManager::HandleCursorPosition(GLFWwindow* window, double xPos, double yPos) 
{
	if (EngineUIMain::AddCursorPosEvent(xPos, yPos))
	{
		if (mouseFirstMoved) {
			lastX = static_cast<float>(xPos);
			lastY = static_cast<float>(yPos);
			mouseFirstMoved = false;
		}

		xChange = static_cast<float>(xPos) - lastX;
		yChange = lastY - static_cast<float>(yPos);

		lastX = static_cast<float>(xPos);
		lastY = static_cast<float>(yPos);
	}
}

void EngineInputManager::HandleMousePresses(GLFWwindow* window, int button, int action, int mode) 
{
	if (EngineUIMain::AddMouseButtonEvent(button, action == GLFW_PRESS)) 
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			if (action == GLFW_PRESS) {
				isLeftMousePress = true;
				isLeftMouseRelease = false;
			}
			else if (action == GLFW_RELEASE) {
				isLeftMouseRelease = true;
				isLeftMousePress = false;
			}
		}

		if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			if (action == GLFW_PRESS) {
				isMiddleMousePress = true;
				isLeftMouseRelease = false;
			}
			else if (action == GLFW_RELEASE) {
				isLeftMouseRelease = true;
				isMiddleMousePress = false;
			}
		}
	}
}

void EngineInputManager::HandleMouseScrolls(GLFWwindow* window, double xOffset, double yOffset)
{
	if (EngineUIMain::AddMouseScrollEvent(xOffset, yOffset)) 
	{
		scrollVal = yOffset;
	}
}

EngineInputManager::~EngineInputManager()
{
	//empty
}
