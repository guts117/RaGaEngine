#ifndef WINDOW
#define WINDOW

#include "pch.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class EngineInputManager;

		class Window
		{
			friend class EngineInputManager;
		public:
			explicit Window();

			int Initialise();

			inline bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }
			inline bool* getKeys() { return keys; }
			GLfloat getXChange();
			GLfloat getYChange();

			glm::vec2 GetCursorPos();

			inline void swapBuffers() { glfwSwapBuffers(mainWindow); }
			inline void SetCursorActive(bool isActive) { glfwSetInputMode(mainWindow, GLFW_CURSOR, isActive ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN); }
			inline void ResizeWindow(GLuint x, GLuint y) { glfwSetWindowSize(mainWindow, x, y); }

			bool isLeftMousePress;
			bool isLeftMouseRelease;

			inline GLFWwindow* GetWindow() { return mainWindow; }

			~Window();

		private:
			GLFWwindow* mainWindow = nullptr;

			void createCallbacks();

			bool keys[1024] = { false };

			GLfloat lastX;
			GLfloat lastY;
			GLfloat xChange;
			GLfloat yChange;
			bool mouseFirstMoved = true;
		};
	}
}
#endif