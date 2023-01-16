#ifndef ENGINE_INPUT_MANAGER
#define ENGINE_INPUT_MANAGER

#include "pch.h"

namespace NarakaKarEngine
{
	namespace InputManager
	{
		class EngineInputManager
		{
		public:
			EngineInputManager();

			~EngineInputManager();

			static inline bool* GetKeys() { return keys; }
			static GLfloat GetXChange();
			static GLfloat GetYChange();
			static glm::vec2 GetCursorPos();
			static inline bool IsLeftMousePress() { return isLeftMousePress; }
			static inline bool IsLeftMouseRelease() { return isLeftMouseRelease; }
			static inline bool IsMiddleMousePress() { return isMiddleMousePress; }
			static inline bool IsMiddleMouseRelease() { return isMiddleMouseRelease; }

			static double GetScrollVal() { double tempScroll = scrollVal; scrollVal = 0; return tempScroll; }

			static void HandleKeysPresses(GLFWwindow* window, int key, int code, int action, int mode);
			static void HandleCursorPosition(GLFWwindow* window, double xPos, double yPos);
			static void HandleMousePresses(GLFWwindow* window, int button, int action, int mode);
			static void HandleMouseScrolls(GLFWwindow* window, double xOffset, double yOffset);

		private:

			static bool keys[1024];

			static bool isLeftMousePress;
			static bool isLeftMouseRelease;
			static bool isMiddleMousePress;
			static bool isMiddleMouseRelease;

			static double scrollVal;

			static GLfloat lastX;
			static GLfloat lastY;
			static GLfloat xChange;
			static GLfloat yChange;
			static bool mouseFirstMoved;
		};
	}
}
#endif