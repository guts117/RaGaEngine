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

			bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }
			bool* getKeys() { return keys; }
			GLfloat getXChange();
			GLfloat getYChange();

			glm::vec2 GetCursorPos();

			void swapBuffers() { glfwSwapBuffers(mainWindow); }
			void SetCursorActive(bool isActive) { glfwSetInputMode(mainWindow, GLFW_CURSOR, isActive ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED); }

			bool isLeftMousePress;
			bool isLeftMouseRelease;

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