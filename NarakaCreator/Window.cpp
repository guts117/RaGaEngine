#include "creator_pch.h"
#include "Window.h"
#include "EngineUIMain.h"

using namespace NarakaCreator;
using namespace EngineUI;

//[3840 x 2160] [2560 x 1440] [1920 x 1080] [1280x720]
int ScreenWidth;
int ScreenHeight;
bool IsUpdateFrameBuffersSize;

Window::Window() = default;

static int Mini(int x, int y)
{
	return x < y ? x : y;
}

static int Maxi(int x, int y)
{
	return x > y ? x : y;
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
			Maxi(0, Mini(wx + ww, mx + mw) - Maxi(wx, mx)) *
			Maxi(0, Mini(wy + wh, my + mh) - Maxi(wy, my));

		if (bestoverlap < overlap) {
			bestoverlap = overlap;
			bestmonitor = monitors[i];
		}
	}

	return bestmonitor;
}

int Window::Initialise() 
{
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
	mainWindow = glfwCreateWindow(mode->width, mode->height, "NarakaCreator", NULL, NULL);

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
			IsUpdateFrameBuffersSize = true;
		});

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

void Window::CreateInputCallbacks(const GLFWkeyfun& keyCallback, const GLFWcursorposfun& cursorPosCallback, const GLFWmousebuttonfun& mouseBtnCallback, const GLFWscrollfun& mouseScrollCallback)
{
	glfwSetKeyCallback(mainWindow, keyCallback);
	glfwSetCursorPosCallback(mainWindow, cursorPosCallback);
	glfwSetMouseButtonCallback(mainWindow, mouseBtnCallback);
	glfwSetScrollCallback(mainWindow, mouseScrollCallback);
}

Window::~Window() {
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}