#ifndef WINDOW
#define WINDOW

#include "pch.h"

class EngineInputManager;

class Window
{
	friend class EngineInputManager;
public:
	Window();
	Window(GLint windowWidth, GLint windowheight);

	int Initialise();

	bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }

	GLint getBufferWidth() { return bufferWidth; }
	GLint getBufferHeight() { return bufferHeight; }

	bool* getKeys() { return keys; }
	GLfloat getXChange();
	GLfloat getYChange();

	void swapBuffers() { glfwSwapBuffers(mainWindow); }

	~Window();

private:
	GLFWwindow* mainWindow;

	GLint width, height;
	GLint bufferWidth, bufferHeight;

	void createCallbacks();

	bool keys[1024] = {false};

	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	bool mouseFirstMoved = true;
};

#endif