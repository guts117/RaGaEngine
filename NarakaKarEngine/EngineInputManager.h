#ifndef ENGINE_INPUT_MANAGER
#define ENGINE_INPUT_MANAGER

#include "pch.h"

class Window;

class EngineInputManager
{
	friend class Window;
public:
	EngineInputManager();

	~EngineInputManager();

private:

	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	static void handleMouse(GLFWwindow* window, double xPos, double yPos);
};

#endif