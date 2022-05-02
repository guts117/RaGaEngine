#include "Window.h"
#include <stdio.h>


Window::Window() {
	mainWindow = nullptr;
	width = 800;
	height = 600;
	bufferWidth = 0; 
	bufferHeight=0;

	for (size_t i = 0; i < 1024; i++) {
		keys[i] = 0;
	}
}
Window::Window(GLint windowWidth, GLint windowHeight) {
	mainWindow = nullptr;
	width = windowWidth;
	height = windowHeight;
	bufferWidth = 0;
	bufferHeight = 0;

	for (size_t i = 0; i < 1024; i++) {
		keys[i] = 0;
	}
}

int Window::Initialise() {
	//initialize GLFW
	if (!glfwInit()) {
		printf("GLFW initialization failed!");
		glfwTerminate();
		return 1;
	}

	//Setup GLFW window properties
	//OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);// sets OpenGl 3._
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);// sets OpenGl _.3
	//core profile means no backwards compatibility 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	///allow forward compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	mainWindow = glfwCreateWindow(width, height, "Test Window", NULL, NULL);

	if (!mainWindow) {
		printf("GLFWwindow creation failed");
		glfwTerminate();
		return 1;
	}

	//get buffer size information
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//Handle key+ Mouse Input
	createCallbacks();
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hides the cursor

	//set context for GLEW to use
	glfwMakeContextCurrent(mainWindow);

	//allow modern extension features
	glewExperimental = GL_TRUE;

	GLenum error = glewInit();
	if (error != GLEW_OK) {
		printf("Error:%s", glewGetErrorString(error));
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	//glFrontFace(GL_CCW);
	//glCullFace(GL_BACK);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	//setup viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);
	glfwSetWindowUserPointer(mainWindow, this); // passing in our mainWindow and the user of that window 

	return 0;
}


void  Window::createCallbacks() {
	glfwSetKeyCallback(mainWindow, InputManager::handleKeys);
	glfwSetCursorPosCallback(mainWindow, InputManager::handleMouse);
}

GLfloat  Window::getXChange() {
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat  Window::getYChange() {
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

Window::~Window() {
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}