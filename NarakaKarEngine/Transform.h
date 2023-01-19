#ifndef TRANSFORM
#define TRANSFORM

namespace NarakaKarEngine
{
	struct Transform
	{
		glm::vec3 position;
		glm::quat quaternion;
		glm::vec3 eulerAngles;
		glm::vec3 Scale;

		glm::quat localPosition;
		glm::quat localQuaternion;
		glm::vec3 localEulerAngles;
		glm::vec3 localScale;
	};
}

#endif