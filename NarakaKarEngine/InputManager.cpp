#include "InputManager.h"

InputManager::InputManager()
{
	//empty
}

void InputManager::handleKeys(GLFWwindow* window, int key, int code, int action, int mode) {
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window)); // grabbing that user pointer and casting it to Window*

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			theWindow->keys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			theWindow->keys[key] = false;
		}
	}
}
void InputManager::handleMouse(GLFWwindow* window, double xPos, double yPos) {
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

InputManager::~InputManager()
{
	//empty
}
