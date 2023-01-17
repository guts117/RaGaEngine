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

			void CreateInputCallbacks(const GLFWkeyfun& keyCallback, const GLFWcursorposfun& cursorPosCallback, const GLFWmousebuttonfun& mouseBtnCallback, const GLFWscrollfun& mouseScrollCallback);
			inline void swapBuffers() { glfwSwapBuffers(mainWindow); }
			inline void SetCursorActive(bool isActive) 
			{
				if (isMouseActive != isActive) 
				{ 
					std::cout << "change" << std::endl;
					isMouseActive = isActive; 
					glfwSetInputMode(mainWindow, GLFW_CURSOR, isActive ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN); 
				} 
			}
			inline bool IsCursorHidden() { return glfwGetInputMode(mainWindow, GLFW_CURSOR) == GLFW_CURSOR_HIDDEN; }
			inline void ResizeWindow(GLuint x, GLuint y) { glfwSetWindowSize(mainWindow, x, y); }

			inline GLFWwindow* GetWindow() { return mainWindow; }

			~Window();

		private:
			GLFWwindow* mainWindow = nullptr;
		    bool isMouseActive;
		};
	}
}
#endif

GLFWmonitor* Get_current_monitor(GLFWwindow* window);
