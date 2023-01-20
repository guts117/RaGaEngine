#ifndef TRANSFORM
#define TRANSFORM

#include "pch.h"

namespace NarakaKarEngine
{
	struct Transform
	{
		glm::vec3 position;
		glm::quat quaternion;
		glm::vec3 eulerAngles;
		glm::vec3 scale;

		glm::quat localPosition;
		glm::quat localQuaternion;
		glm::vec3 localEulerAngles;
		glm::vec3 localScale;

		glm::mat4 model_matrix;
		glm::mat4 prev_matrix;

		void Translate(glm::vec3&& position);
		void RotateOnAxis(float&& angleInDegrees, glm::vec3&& axis);
		void Scale(glm::vec3&& scale);
		void UpdateModelMatrix();
	};
}

#endif