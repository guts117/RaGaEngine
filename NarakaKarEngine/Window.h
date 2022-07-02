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

			void swapBuffers() { glfwSwapBuffers(mainWindow); }

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