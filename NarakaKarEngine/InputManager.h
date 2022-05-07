#pragma once
#include "Window.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class InputManager
{
	friend class Window;
public:
	InputManager();

	~InputManager();

private:

	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	static void handleMouse(GLFWwindow* window, double xPos, double yPos);
};

