#include "pch.h"
#include "EngineInputManager.h"
#include "Window.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

EngineInputManager::EngineInputManager()
{
	//empty
}

void EngineInputManager::handleKeys(GLFWwindow* window, int key, int code, int action, int mode) {
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window)); // grabbing that user pointer and casting it to Window*

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			theWindow->keys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			theWindow->keys[key] = false;
		}
	}
}
void EngineInputManager::handleMouse(GLFWwindow* window, double xPos, double yPos) {
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window)); // grabbing that user pointer and casting it to Window*

	if (theWindow->mouseFirstMoved) {
		theWindow->lastX = static_cast<float>(xPos);
		theWindow->lastY = static_cast<float>(yPos);
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = static_cast<float>(xPos) - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - static_cast<float>(yPos);

	theWindow->lastX = static_cast<float>(xPos);
	theWindow->lastY = static_cast<float>(yPos);
}

void EngineInputManager::handleMouseClick(GLFWwindow* window, int button, int action, int mode) {
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window)); // grabbing that user pointer and casting it to Window*

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			theWindow->isLeftMousePress = true;
			theWindow->isLeftMouseRelease = false;
		}
		else if (action == GLFW_RELEASE) {
			theWindow->isLeftMouseRelease = true;
			theWindow->isLeftMousePress = false;
		}
	}
}

EngineInputManager::~EngineInputManager()
{
	//empty
}
