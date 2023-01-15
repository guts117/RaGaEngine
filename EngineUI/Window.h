#ifndef WINDOW
#define WINDOW

#include "engine_ui_pch.h"

namespace NarakaKarEngine
{
	namespace EngineUI
	{
		class Window
		{
		public:
			explicit Window();

			void Update(GLFWwindow* window, int width, int height);

			int Initialise();

			inline bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }
			inline bool* getKeys() { return keys; }
			GLfloat getXChange();
			GLfloat getYChange();

			glm::vec2 GetCursorPos();

			void CreateInputCallbacks(const GLFWkeyfun& keyCallback, const GLFWcursorposfun& cursorPosCallback, const GLFWmousebuttonfun& mouseBtnCallback, const GLFWscrollfun& mouseScrollCallback);
			inline void swapBuffers() { glfwSwapBuffers(mainWindow); }
			inline void SetCursorActive(bool isActive) { if (isMouseActive != isActive) { isMouseActive = isActive; glfwSetInputMode(mainWindow, GLFW_CURSOR, isActive ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN); } }
			inline bool IsCursorHidden() { return glfwGetInputMode(mainWindow, GLFW_CURSOR) == GLFW_CURSOR_HIDDEN; }
			inline void ResizeWindow(GLuint x, GLuint y) { glfwSetWindowSize(mainWindow, x, y); }

			bool isLeftMousePress;
			bool isLeftMouseRelease;
			bool isMiddleMousePress;
			bool isMiddleMouseRelease;

			double scrollVal;

			inline GLFWwindow* GetWindow() { return mainWindow; }

			~Window();

		private:
			GLFWwindow* mainWindow = nullptr;

			bool keys[1024] = { false };

			GLfloat lastX;
			GLfloat lastY;
			GLfloat xChange;
			GLfloat yChange;
		    bool isMouseActive;
			bool mouseFirstMoved = true;
		};
	}
}
#endif

GLFWmonitor* Get_current_monitor(GLFWwindow* window);
