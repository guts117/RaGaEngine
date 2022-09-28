#include "pch.h"
#include "Window.h"
#include "EngineInputManager.h"
#include "RenderingCommonValues.h"
#include "RenderEngineMain.h"
#include "MathUtil.h"
#include "EngineUIMain.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;
using namespace EngineUI;

//[3840 x 2160] [2560 x 1440] [1920 x 1080] [1280x720]
int ScreenWidth;
int ScreenHeight;
bool isUpdateFrameBuffersSize;

Window::Window() {
	for (size_t i = 0; i < 1024; i++) {
		keys[i] = 0;
	}
}

GLFWmonitor* Get_current_monitor(GLFWwindow* window)
{
	int nmonitors, i;
	int wx, wy, ww, wh;
	int mx, my, mw, mh;
	int overlap, bestoverlap;
	GLFWmonitor* bestmonitor;
	GLFWmonitor** monitors;
	const GLFWvidmode* mode;

	bestoverlap = 0;
	bestmonitor = NULL;

	glfwGetWindowPos(window, &wx, &wy);
	glfwGetWindowSize(window, &ww, &wh);
	monitors = glfwGetMonitors(&nmonitors);

	for (i = 0; i < nmonitors; i++) {
		mode = glfwGetVideoMode(monitors[i]);
		glfwGetMonitorPos(monitors[i], &mx, &my);
		mw = mode->width;
		mh = mode->height;

		overlap =
			MathUtil::Maxi(0, MathUtil::Mini(wx + ww, mx + mw) - MathUtil::Maxi(wx, mx)) *
			MathUtil::Maxi(0, MathUtil::Mini(wy + wh, my + mh) - MathUtil::Maxi(wy, my));

		if (bestoverlap < overlap) {
			bestoverlap = overlap;
			bestmonitor = monitors[i];
		}
	}

	return bestmonitor;
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);// sets OpenGl 3._
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);// sets OpenGl _.3
	//core profile means no backwards compatibility 
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	///allow forward compatibility
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	auto mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	mainWindow = glfwCreateWindow(mode->width, mode->height, "NarakaKarEngine", NULL, NULL);

	if (!mainWindow) {
		printf("GLFWwindow creation failed");
		glfwTerminate();
		return 1;
	}
	 
	//get buffer size information
	glfwGetFramebufferSize(mainWindow, &ScreenWidth, &ScreenHeight);

	glfwSetFramebufferSizeCallback(mainWindow, [](GLFWwindow* window, int width, int height)
		{
			printf("Framebuffer size : %d , %d \n", width, height);
			ScreenWidth = width;
			ScreenHeight = height;
			isUpdateFrameBuffersSize = true;
		});

	//Handle key+ Mouse Input
	createCallbacks();
	//glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hides the cursor

	//set context for GLEW to use
	glfwMakeContextCurrent(mainWindow);

	////allow modern extension features
	//glewExperimental = GL_TRUE;

	GLenum error = glewInit();
	if (error != GLEW_OK) {
		printf("Error:%s", glewGetErrorString(error));
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	//setup viewport size
	glViewport(0, 0, ScreenWidth, ScreenHeight);
	glfwSetWindowUserPointer(mainWindow, this); // passing in our mainWindow and the user of that window 

	return 0;
}


void  Window::createCallbacks() {
	glfwSetKeyCallback(mainWindow, EngineInputManager::handleKeys);
	glfwSetCursorPosCallback(mainWindow, EngineInputManager::handleMouse);
	glfwSetMouseButtonCallback(mainWindow, EngineInputManager::handleMouseClick);
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

glm::vec2 Window::GetCursorPos()
{
	double x, y;
	glfwGetCursorPos(mainWindow, &x, &y);
	return glm::vec2(x, y);
}

Window::~Window() {
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}